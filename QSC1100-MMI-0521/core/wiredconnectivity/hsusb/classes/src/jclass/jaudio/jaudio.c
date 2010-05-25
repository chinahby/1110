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
#include <jusb.h>
#include <usbdi.h>
#include <usbdivar.h>
#include <usbdi_util.h>
/* HSU Addition */
#include <jappmgr.h>
/* HSU Addition */
#include <uw_host_audio.h>
#include <lists.h>
#include "jaudio.h"

/* Driver methods - need to be implemented for driver registration with the 
 * host stack's USB Core */
static jresult_t jaudio_probe(jdevice_t device);
static jresult_t jaudio_attach(jdevice_t self);
static jresult_t jaudio_detach(jdevice_t self);
static jresult_t jaudio_suspend(jdevice_t self);
static jresult_t jaudio_resume(jdevice_t self);
static jresult_t jaudio_ioctl(jdevice_t self, drv_ioctl_t ioctl, void *arg);

/* HSU addition */
static void* appmgr = NULL;
/* HSU addition */

#define PIPE_GET_PACKET_LEN(pipe)  \
    (UGETW((pipe)->endpoint->edesc->wMaxPacketSize) & UE_SIZE_VALID)

/**
 * Function name:  usbd_status_to_result
 * Description:    Converts USBD status codes to jresult_t USBware codes.
 * Parameters:
 *     @status: (IN) USBD status code.
 *
 * Return value:   USBware status code (jresult_t).
 * Scope:          Local
 **/
static jresult_t usbd_status_to_result(usbd_status status)
{
    switch(status)
    {
    case USBD_NORMAL_COMPLETION:
    case USBD_IN_PROGRESS:
        return 0;

    case USBD_NOMEM:
        return JENOMEM;

    case USBD_NOT_CONFIGURED:
        return JENXIO;

    case USBD_INVAL:
    case USBD_INVALID_STATE:
        return JEINVAL;

    case USBD_IOERROR:
        return JEIO;
        
    case USBD_TIMEOUT:
        return JETIMEDOUT;

    case USBD_PENDING_REQUESTS:
    case USBD_NOT_STARTED:
    case USBD_CANCELLED:
    case USBD_BAD_ADDRESS:
    case USBD_IN_USE:
    case USBD_NO_ADDR:
    case USBD_SET_ADDR_FAILED:
    case USBD_NO_POWER:
    case USBD_TOO_DEEP:
    case USBD_SHORT_XFER:
    case USBD_STALLED:
    case USBD_INTERRUPTED:
    case USBD_TEST_FAILURE:
    default:
        return EUNKNOWN;
    }
}

/* Streaming methods */
static jresult_t audio_in_stream_open(usbd_device_handle dev, 
    usbd_interface_handle iface, juint8_t pipe_addr, juint32_t buf_time_frames, 
    juint8_t flags, audio_stream_h *stream);

static void audio_stream_close(audio_stream_h stream);
static void audio_stream_stop(audio_stream_h stream);

/* Input stream specific */
static jresult_t audio_in_stream_start(audio_stream_h stream, 
    audio_stream_cb_t cb, void *arg);

static void     audio_in_stream_clear(audio_stream_h stream);

static juint32_t audio_in_stream_get_data_size(audio_stream_h stream);

static void     audio_in_stream_read(audio_stream_h stream, void *buf, 
    juint32_t *len);

/* Output stream specific */
static jresult_t audio_out_stream_open(usbd_device_handle dev, 
    usbd_interface_handle iface, juint8_t pipe_addr, juint16_t audio_frame_size, 
    juint32_t sam_freq, juint8_t flags, audio_stream_h *stream);

/* TODO explanation:  use audio_out_stream_write with data_len 0 to flush */
static jresult_t audio_out_stream_write(audio_stream_h stream, void *buf, 
    juint32_t data_len, audio_stream_cb_t cb, void *arg);

static juint32_t get_isoc_xfer_len(juint16_t *fr_len, juint32_t fr_num);

/* Streaming implementation */

/**
 * Function name:  free_audio_xfer
 * Description:    Frees memory allocated for a given audio transfer.
 * Parameters:
 *     @axfer: (IN) Audio transfer handle.
 *
 * Return value:   None
 * Scope:          Local
 **/
static void free_audio_xfer(audio_xfer_h axfer)
{
    KASSERT(axfer, ("Trying to free NULL xfer\n"));

    DBG_X(DHOST_AUDIO, ("AUDIO: free_audio_xfer - %p\n", axfer));

    if (!(axfer->xfer->flags & USBD_DMA_BUF) && usbd_get_buffer(axfer->xfer))
        usbd_free_buffer(axfer->xfer);

    if (axfer->fr_len)
        jfree(axfer->fr_len);

    if (axfer->xfer)
        usbd_free_xfer(axfer->xfer);

    jfree(axfer);    
}

/**
 * Function name:  alloc_audio_xfer
 * Description:    Allocates memory for an isochronous audio transfer.
 * Parameters:
 *     @stream:    (IN) Stream handle.
 *     @frames:    (IN) Frame number.
 *     @frame_len: (IN) Frame length, in bytes.
 *
 * Return value:   Handle to the allocated audio transfer, or NULL on failure.
 * Scope:          Local
 **/
static audio_xfer_h alloc_audio_xfer(audio_stream_h stream, juint32_t frames,
    juint32_t frame_len)
{
    audio_xfer_h axfer;
    juint32_t axfer_len = STREAM_IS_IN(stream) ? sizeof(audio_xfer_t) : 
        sizeof(audio_out_xfer_t);

    axfer = (audio_xfer_h)jmalloc(axfer_len, M_ZERO);
    if (!axfer)
        return NULL;

    DBG_X(DHOST_AUDIO, ("AUDIO: alloc_audio_xfer - %p\n", axfer));
    
    axfer->xfer = usbd_alloc_xfer(stream->dev);
    if (!axfer->xfer)
        goto Error;

    axfer->fr_len = (juint16_t *)jmalloc(frames * sizeof(juint16_t), 0);
    if (!axfer->fr_len)
        goto Error;

    if (frame_len)
    {
        usbd_alloc_buffer(axfer->xfer, frames * frame_len);
        if (!usbd_get_buffer(axfer->xfer))
            goto Error;
    }

    axfer->stream = stream;

    return axfer;

Error:
    free_audio_xfer(axfer);
    return NULL;
}

/**
 * Function name:  update_axfer_completion_stat
 * Description:    Updates a transfer's completion status by ignoring the
 *                 specified errors .
 * Parameters:
 *     @stream: (IN)     Stream handle.
 *     @stat:   (IN/OUT) Pointer to a transfer completion status.
 *
 * Return value:   None
 * Scope:          Local
 **/
static void update_axfer_completion_stat(audio_stream_h stream, 
    usbd_status *stat)
{
    KASSERT(stream, ("Stream handle is NULL\n"));

    if (stream->flags & AUDIO_STREAM_IGNORE_EIO)
    {
        switch (*stat)
        {
        case USBD_SHORT_XFER:
        case USBD_IOERROR:
            DBG_W(DHOST_AUDIO_STREAM, ("AUDIO: Stream (%p), ignored error - %s"
                "\n", stream, usbd_errstr(*stat)));
            *stat = USBD_NORMAL_COMPLETION;
            break;
        default:
            break;
        }
    }
}

static jresult_t audio_in_stream_reschedule(audio_stream_h stream);
static void in_stream_task(void *stream_h);

/**
 * Function name:  in_axfer_complete
 * Description:    Audio transfer completion callback.
 * Parameters:
 *     @xfer: (IN) xfer handle
 *     @priv: (IN) Audio transfer handle
 *     @stat: (IN) transfer completion status
 *
 * Return value:   None
 * Scope:          Local
 **/
static void in_axfer_complete(usbd_xfer_handle xfer, 
    usbd_private_handle axfer_handle, usbd_status stat)
{
    audio_xfer_h axfer = (audio_xfer_h)axfer_handle;
    audio_stream_h stream;
    audio_in_stream_h istream;
    jresult_t rc;

    KASSERT(axfer,  ("axfer is NULL\n"));

    stream = axfer->stream;
    istream = GET_IN_STREAM(stream);

    KASSERT(stream, ("Stream handle is NULL\n"));
    KASSERT(istream->axfers.active,
        ("Received callback without active axfer\n"));

    DBG_X(DHOST_AUDIO_STREAM, ("AUDIO: in_axfer_complete - %p, stat=%d\n",
        axfer, stat));

    istream->axfers.active--;

    if (!(stream->flags & (AUDIO_STREAM_STARTED | AUDIO_STREAM_TRY_START)))
    {
        STAILQ_INSERT_TAIL(&istream->axfers.free, axfer, next);
        return;
    }

    DBG_IF (stat == USBD_NORMAL_COMPLETION && !istream->axfers.active)
        DBG_W(DHOST_AUDIO, ("No active xfers, possible stream out of sync\n"));

    update_axfer_completion_stat(stream, &stat);
         
    if (stat != USBD_NORMAL_COMPLETION)
    {
        DBG_E(DHOST_AUDIO,("AUDIO: Stream (%p) stopped, error - %s\n", stream,
            usbd_errstr(stat)));
        
        STAILQ_INSERT_TAIL(&istream->axfers.free, axfer, next);
        
        audio_stream_stop(stream);
        
        istream->error = usbd_status_to_result(stat);

        if (stream->flags & AUDIO_STREAM_TRY_START)
            return;

        rc = jtask_schedule(stream->task, 0, in_stream_task, stream);
        KASSERT(!rc, ("AUDIO: Can't start task\n"));

        return;
    }

    if (!STAILQ_FIRST(&istream->axfers.done))
    {
        istream->done_frame = 0;
        istream->done_in_frame = 0;
    }

    if (get_isoc_xfer_len(axfer->fr_len, istream->frames_in_buf))
    {
        STAILQ_INSERT_TAIL(&istream->axfers.done, axfer, next);
    }
    else
    {
        STAILQ_INSERT_TAIL(&istream->axfers.free, axfer, next);
        rc = audio_in_stream_reschedule(stream);
        if (!rc)
            return;

        istream->error = rc;
    }

    rc = jtask_schedule(stream->task, 0, in_stream_task, stream);
    KASSERT(!rc, ("AUDIO: Can't start task\n"));
}

/**
 * Function name:  restart_in_axfer
 * Description:    Restarts a given audio transfer.
 * Parameters:
 *     @stream: (IN) Stream handle.
 *     @axfer:  (IN) Audio transfer handle
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t restart_in_axfer(audio_stream_h stream, audio_xfer_h axfer)
{
    juint32_t i;
    juint16_t packet_len;
    usbd_status stat;
    audio_in_stream_h istream = GET_IN_STREAM(stream);

    KASSERT(stream, ("Stream handle is NULL\n"));
    KASSERT(axfer,  ("axfer is NULL\n"));

    DBG_X(DHOST_AUDIO_STREAM, ("AUDIO: restart_in_axfer - %p\n", axfer));

    packet_len = PIPE_GET_PACKET_LEN(stream->pipe);
    
    for (i = 0; i < istream->frames_in_buf; i++)
        axfer->fr_len[i] = packet_len;

    usbd_setup_isoc_xfer(axfer->xfer, stream->pipe, axfer, axfer->fr_len,
        istream->frames_in_buf, USBD_NO_COPY, in_axfer_complete);

    axfer->xfer->length = packet_len * istream->frames_in_buf;

    stat = usbd_transfer(axfer->xfer);

    istream->axfers.active++;

    if (stat != USBD_IN_PROGRESS && stat != USBD_NORMAL_COMPLETION)
    {
        istream->axfers.active--;
        DBG_E(DHOST_AUDIO_STREAM,("AUDIO: start xfer failed, stat=%d\n", stat));
    }

    return usbd_status_to_result(stat);
}

/**
 * Function name:  audio_in_stream_get_status
 * Description:    Returns a given stream's current status.
 * Parameters:
 *     @stream: (IN) Stream handle.
 *
 * Return value:   Stream status: 0 = OK; otherwise an error code.
 * Scope:          Global
 **/
jresult_t audio_in_stream_get_status(audio_stream_h stream)
{
    KASSERT(stream, ("Stream handle is NULL\n"));

    return GET_IN_STREAM(stream)->error;
}

/**
 * Function name:  in_stream_task
  Description:     Stream notification and transfer completion handling task
 * Parameters:
 *     @stream_h: (IN) Stream handle.
 *
 * Return value:   None
 * Scope:          Local
 **/
static void in_stream_task(void *stream_h)
{
    audio_stream_h stream = (audio_stream_h)stream_h;
    audio_in_stream_h istream = GET_IN_STREAM(stream);
    jresult_t rc;

    KASSERT(stream, ("Stream handle is NULL\n"));

    if (istream->error || STAILQ_FIRST(&istream->axfers.done))
    {
        if (istream->cb)
            istream->cb(istream->arg, istream->error);
    }

    if (!(stream->flags & AUDIO_STREAM_STARTED))
        return;
        
    if (!istream->axfers.active && !(stream->flags & AUDIO_STREAM_IGNORE_EIO))
    {
        rc = JETIMEDOUT;
        goto Error;
    }

    if (STAILQ_FIRST(&istream->axfers.free))
    {
        rc = audio_in_stream_reschedule(stream);
        if (rc)
            goto Error;
    }

    return;

Error:
    audio_stream_stop(stream);

    DBG_E(DHOST_AUDIO,("AUDIO: Stream (%p) stopped, error=%d\n", stream, rc));

    istream->error = rc;

    if (istream->cb)
        istream->cb(istream->arg, rc);
}

/**
 * Function name:  audio_in_stream_reschedule
 * Description:    Reschedule stream transfers
 * Parameters:
 *     @stream: (IN) Stream handle.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t audio_in_stream_reschedule(audio_stream_h stream)
{
    audio_xfer_h axfer;
    jresult_t rc = 0;
    audio_in_stream_h istream = GET_IN_STREAM(stream);

    KASSERT(stream, ("Stream handle is NULL\n"));
    
    DBG_X(DHOST_AUDIO_STREAM, ("AUDIO: audio_in_stream_reschedule - %p, "
        "active xfers - %d\n", stream, istream->axfers.active));

    while (!rc && (axfer = STAILQ_FIRST(&istream->axfers.free)) != NULL)
    {
        STAILQ_REMOVE_HEAD(&istream->axfers.free, next);
        
        rc = restart_in_axfer(stream, axfer);
    }

    return rc;
}

/**
 * Function name:  audio_in_stream_start
 * Description:    Start streaming
 * Parameters:
 *     @stream: (IN) Stream handle.
 *     @cb:     (IN) transfer completion callback
 *     @arg:    (IN) User context.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t audio_in_stream_start(audio_stream_h stream, 
    audio_stream_cb_t cb, void *arg)
{
    jresult_t rc;
    audio_in_stream_h istream = GET_IN_STREAM(stream);

    KASSERT(stream, ("Stream handle is NULL\n"));
    KASSERT(STREAM_IS_IN(stream), ("Can't be used for output stream!\n"));

    DBG_V(DHOST_AUDIO_STREAM, ("AUDIO: audio_in_stream_start - %p\n", stream));

    istream->error = 0;

    if (stream->flags & (AUDIO_STREAM_TRY_START | AUDIO_STREAM_STARTED))
        return 0;
        
    stream->flags |= AUDIO_STREAM_TRY_START;
    istream->cb  = cb;
    istream->arg = arg;

    rc = audio_in_stream_reschedule(stream);
    
    stream->flags &= ~AUDIO_STREAM_TRY_START;
    
    if (!rc)
        rc = istream->error;

    if (rc)
    {
        istream->error = rc;
        usbd_abort_pipe(stream->pipe);
        return rc;
    }

    stream->flags |= AUDIO_STREAM_STARTED;
    
    return 0;
}

/**
 * Function name:  audio_in_stream_clear
 * Description:    Remove received data from stream buffer
 * Parameters:
 *     @stream: (IN) Stream handle.
 *
 * Return value:   None
 * Scope:          Local
 **/
static void audio_in_stream_clear(audio_stream_h stream)
{
    audio_xfer_h axfer;
    audio_in_stream_h istream = GET_IN_STREAM(stream);

    KASSERT(stream, ("Stream handle is NULL\n"));
    KASSERT(STREAM_IS_IN(stream), ("Can't be used for output stream!\n"));

    DBG_V(DHOST_AUDIO_STREAM, ("AUDIO: audio_in_stream_clear - %p\n", stream));
    
    DBG_IF(stream->flags & AUDIO_STREAM_STARTED)
    {
        DBG_W(DHOST_AUDIO_STREAM, ("AUDIO: audio_in_stream_clear - %p, warning:"
            " stream started\n", stream));
    }

    while ((axfer = STAILQ_FIRST(&istream->axfers.done)) != NULL)
    {
        STAILQ_REMOVE_HEAD(&istream->axfers.done, next);
        STAILQ_INSERT_TAIL(&istream->axfers.free, axfer, next);
    }
    istream->done_frame = 0;
    istream->done_in_frame = 0;
}

/**
 * Function name:  audio_stream_stop
 * Description:    Stop streaming (received data still available)
 * Parameters:
 *     @stream: (IN) Stream handle.
 *
 * Return value:   None
 * Scope:          Local
 **/
static void audio_stream_stop(audio_stream_h stream)
{
    KASSERT(stream, ("Stream handle is NULL\n"));

    DBG_V(DHOST_AUDIO_STREAM, ("AUDIO: audio_stream_stop - %p\n", stream));

    if (STREAM_IS_IN(stream))
    {
        if (!(stream->flags & AUDIO_STREAM_STARTED))
            return;

        stream->flags &= ~AUDIO_STREAM_STARTED;
    }

    usbd_abort_pipe(stream->pipe);

    if (!STREAM_IS_IN(stream))
    {
        if (GET_OUT_STREAM(stream)->rem_axfer)
            free_audio_xfer(GET_OUT_STREAM(stream)->rem_axfer);
        GET_OUT_STREAM(stream)->rem_axfer = NULL;
    }
}

static void audio_stream_uninit(audio_stream_h stream)
{
    KASSERT(stream, ("Stream handle is NULL\n"));

    DBG_I(DHOST_AUDIO_STREAM, ("AUDIO: audio_stream_uninit - %p\n", stream));

    if (stream->pipe)
        usbd_close_pipe(stream->pipe);

    if (stream->task)
        jtask_uninit(stream->task);

    jfree(stream);
}

static jresult_t audio_stream_init(usbd_device_handle dev, 
    usbd_interface_handle iface, juint8_t pipe_addr, juint8_t flags, 
    juint32_t stream_size, audio_stream_h *stream)
{
    usbd_status stat;
    jresult_t rc;
    audio_stream_h s;

    KASSERT(stream, ("Stream arg is NULL\n"));

    s = (audio_stream_h)jmalloc(stream_size, M_ZERO);
    if (!s)
    {
        DBG_E(DHOST_AUDIO_STREAM, ("AUDIO: audio_stream_init pipe(%02x), "
            "no memory\n", pipe_addr));
        return JENOMEM;
    }

    DBG_I(DHOST_AUDIO_STREAM, ("AUDIO: audio_stream_init - %p, "
        "pipe(%02x)\n", s, pipe_addr));

    s->dev   = dev;
    s->iface = iface;
    s->flags = flags;
    
    rc = jtask_init(&s->task, s->flags & AUDIO_STREAM_CLASS_TASK ?
        TASK_CLASS_DRIVER : TASK_CORE);
    if (rc)
    {
        DBG_E(DHOST_AUDIO_STREAM, ("AUDIO: audio_stream_init - %p, task "
            "init failed, error=%d\n", s, rc)); 
        goto Error;
    }

    stat = usbd_open_pipe(s->iface, pipe_addr, USBD_EXCLUSIVE_USE, 
        &s->pipe);
    if (stat != USBD_NORMAL_COMPLETION)
    {
        DBG_E(DHOST_AUDIO_STREAM, ("AUDIO: audio_stream_init, can't open pipe"
            " %02x error=%d\n", pipe_addr, stat)); 
        rc = usbd_status_to_result(stat);
        goto Error;
    }

    *stream = s;

    return 0;

Error:
    audio_stream_uninit(s);
    return rc;
}

/**
 * Function name:  audio_in_stream_open
 * Description:    Create stream, xfers and open pipe
 * Parameters:
 *     @dev:           (IN) device handle
 *     @iface:         (IN) interface handle
 *     @pipe_addr:     (IN) endpoint address
 *     @buf_time_frames: (IN) stream buffering time, in usb frames
 *     @stream:        (OUT) Stream handle.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t audio_in_stream_open(usbd_device_handle dev, 
    usbd_interface_handle iface, juint8_t pipe_addr, juint32_t buf_time_frames, 
    juint8_t flags, audio_stream_h *stream)
{
    jresult_t rc;
    juint_t i;
    audio_xfer_h axfer;
    audio_stream_h s;
    audio_in_stream_h is;

    KASSERT(stream, ("Stream arg is NULL\n"));

    rc = audio_stream_init(dev, iface, pipe_addr, flags | AUDIO_STREAM_IN, 
        sizeof(audio_in_stream_t), &s);
    if (rc)
        return rc;

    is = GET_IN_STREAM(s);

    if (s->flags & AUDIO_STREAM_USE_DELIMITER)
    {
        is->bufs_num = AUDIO_STREAM_DELIM_MIN_BUFS_NUM;
        is->frames_in_buf = MAX(buf_time_frames, AUDIO_STREAM_MIN_FRAMES);
    }
    else
    {
        is->bufs_num = AUDIO_STREAM_MIN_BUFS_NUM + 
            AUDIO_STREAM_MIN_FRAMES/buf_time_frames;

        is->frames_in_buf = buf_time_frames;
    }

    DBG_I(DHOST_AUDIO_STREAM, ("AUDIO: audio_in_stream_open - %p, bufs=%d, "
        "frames=%d\n", s, is->bufs_num, is->frames_in_buf));

    STAILQ_INIT(&is->axfers.free);
    STAILQ_INIT(&is->axfers.done);

    for (i = 0; i < is->bufs_num; i++)
    {
        axfer = alloc_audio_xfer(s, is->frames_in_buf, 
            PIPE_GET_PACKET_LEN(s->pipe));
        if (!axfer)
        {
            DBG_E(DHOST_AUDIO_STREAM, ("AUDIO: audio_in_stream_open - %p, no "
                "memory\n", s));
            rc = JENOMEM;
            goto Error;
        }

        STAILQ_INSERT_TAIL(&is->axfers.free, axfer, next);
    }

    *stream = s;

    return 0;

Error:
    audio_stream_close(s);
    return rc;
}

static jresult_t audio_out_stream_open(usbd_device_handle dev, 
    usbd_interface_handle iface, juint8_t pipe_addr, juint16_t audio_frame_size, 
    juint32_t sam_freq, juint8_t flags, audio_stream_h *stream)
{
    jresult_t rc;
    audio_stream_h s;
    audio_out_stream_h os;

    KASSERT(stream, ("Stream arg is NULL\n"));

    rc = audio_stream_init(dev, iface, pipe_addr, flags, 
        sizeof(audio_out_stream_t), &s);
    if (rc)
        return rc;

    os = GET_OUT_STREAM(s);

    STAILQ_INIT(&os->done_axfers);
    os->sam_freq = sam_freq;

    os->audio_frame_size = audio_frame_size;
    
    os->isoc_freq = ((s->dev->speed == USB_SPEED_HIGH) ? 8000 : 1000) >> 
        (s->pipe->endpoint->edesc->bInterval - 1);
    
    os->average.div = os->sam_freq / os->isoc_freq;
    os->average.rem = os->sam_freq % os->isoc_freq;
    os->average.sum = 0;

    if (PIPE_GET_PACKET_LEN(s->pipe) < 
        (os->average.div + (os->average.rem != 0)) * os->audio_frame_size)
    {
        DBG_E(DHOST_AUDIO_STREAM, ("AUDIO: audio_out_stream_open - %p failed, "
            "sampling frequency %d Hz and audio frame size %d too high for "
            "isoc frame %d\n", s, os->sam_freq, os->audio_frame_size, 
            PIPE_GET_PACKET_LEN(s->pipe)));
        rc = JEINVAL;
        goto Error;            
    }

    DBG_I(DHOST_AUDIO_STREAM, ("AUDIO: audio_out_stream_open - %p OUT, "
        "sampling frequency %d Hz, audio frame size %d, isoc frame %d(%d %d)\n",
        s, os->sam_freq, os->audio_frame_size, PIPE_GET_PACKET_LEN(s->pipe),
        os->average.div, os->average.rem));

    *stream = s;

    return 0;

Error:
    audio_stream_close(s);
    return rc;
}

/**
 * Function name:  audio_stream_close
 * Description:    Close stream (abort all stream transfers, close pipe, free
 *                 xfers)
 * Parameters:
 *     @stream: (IN) Stream handle.
 *
 * Return value:   None
 * Scope:          Local
 **/
static void audio_stream_close(audio_stream_h stream)
{
    audio_xfer_h axfer;

    KASSERT(stream, ("Stream handle is NULL\n"));

    DBG_I(DHOST_AUDIO_STREAM, ("AUDIO: audio_stream_close - %p\n", stream));

    if (stream->pipe)
    {
        audio_stream_stop(stream);

        if (STREAM_IS_IN(stream))
            audio_in_stream_clear(stream);
    }

    if (STREAM_IS_IN(stream))
    {
        audio_in_stream_h istream = GET_IN_STREAM(stream);

        while ((axfer = STAILQ_FIRST(&istream->axfers.free)) != NULL)
        {
            STAILQ_REMOVE_HEAD(&istream->axfers.free, next);
            free_audio_xfer(axfer);
        }
    }

    audio_stream_uninit(stream);
}

static void out_stream_task(void *stream_h)
{
    audio_stream_h stream = (audio_stream_h)stream_h;
    audio_out_stream_h ostream = GET_OUT_STREAM(stream);
    audio_xfer_h axfer;
    usbd_status stat;

    KASSERT(stream, ("Stream handle is NULL\n"));

    while ((axfer = STAILQ_FIRST(&ostream->done_axfers)) != NULL)
    {
        audio_out_xfer_h out_axfer = GET_OUT_AXFER(axfer);

        STAILQ_REMOVE_HEAD(&ostream->done_axfers, next);
        
        usbd_get_xfer_status(axfer->xfer, NULL, NULL, NULL, &stat);

        DBG_IF(stat != USBD_NORMAL_COMPLETION)
        {
            DBG_W(DHOST_AUDIO_STREAM, ("AUDIO: Stream (%p) transfer failed, "
                "error - %s\n", stream, usbd_errstr(stat)));
        }
        
        KASSERT(out_axfer->cb, ("No xfer callback in completion task\n"));
        
        DBG_X(DHOST_AUDIO_STREAM, ("AUDIO: Write completed, axfer=%p arg=%p\n", 
            axfer, out_axfer->arg));

        out_axfer->cb(out_axfer->arg, usbd_status_to_result(stat));

        free_audio_xfer(axfer);
    }
}

static void out_axfer_complete(usbd_xfer_handle xfer, 
    usbd_private_handle axfer_handle, usbd_status stat);

static usbd_status start_rem_axfer(audio_stream_h stream, void **buffer, 
    juint32_t *len)
{
    juint16_t send_data = 0;
    usbd_status stat;
    audio_out_stream_h ostream = GET_OUT_STREAM(stream);

    if (buffer)
    {
        send_data = ostream->rem_axfer->fr_len[0] - ostream->rem_axfer_len;

        if (*len < send_data)
            send_data = *len;

        j_memcpy((juint8_t *)usbd_get_buffer(ostream->rem_axfer->xfer) + 
            ostream->rem_axfer_len, *buffer, send_data);
        
        ostream->rem_axfer->fr_len[0] = ostream->rem_axfer_len + send_data;

        DBG_X(DHOST_AUDIO_STREAM, ("AUDIO: start_rem_axfer stream(%p), len=%d "
            "[%d,%d]\n", stream, *ostream->rem_axfer->fr_len, 
            ostream->rem_axfer_len, send_data));
    }
    else
    {
        ostream->rem_axfer->fr_len[0] = ostream->rem_axfer_len;
    }

    usbd_setup_isoc_xfer(ostream->rem_axfer->xfer, stream->pipe, 
        ostream->rem_axfer, ostream->rem_axfer->fr_len, 1, USBD_NO_COPY, 
        out_axfer_complete);

    ostream->rem_axfer->xfer->length = ostream->rem_axfer->fr_len[0];

    stat = usbd_transfer(ostream->rem_axfer->xfer);

    if (stat == USBD_IN_PROGRESS)
    {
        ostream->active_axfers++;

        if (buffer)
        {
            *buffer = (juint8_t *)*buffer + send_data;

            *len -= send_data;
        }
    }
    else
    {
        free_audio_xfer(ostream->rem_axfer);

        DBG_E(DHOST_AUDIO_STREAM,("AUDIO: start_rem_axfer (%p) failed, error"
            " - %s\n", stream, usbd_errstr(stat)));
    }

    ostream->rem_axfer = NULL;

    return stat;
}

static void out_axfer_complete(usbd_xfer_handle xfer, 
    usbd_private_handle axfer_handle, usbd_status stat)
{
    audio_xfer_h axfer = (audio_xfer_h)axfer_handle;
    audio_out_xfer_h out_axfer = GET_OUT_AXFER(axfer);
    audio_stream_h stream;
    audio_out_stream_h ostream;
    jresult_t rc;

    KASSERT(axfer, ("axfer is NULL\n"));

    stream = axfer->stream;
    KASSERT(stream, ("Stream handle is NULL\n"));

    ostream = GET_OUT_STREAM(stream);

    DBG_X(DHOST_AUDIO_STREAM, ("AUDIO: out_axfer_complete %p, stat=%d arg=%p\n",
        axfer, stat, out_axfer->arg));
    
    DBG_IF (stat != USBD_NORMAL_COMPLETION)
    {
        DBG_E(DHOST_AUDIO_STREAM,("AUDIO: OUT transfer (%p) failed, error=%s\n",
            axfer, usbd_errstr(stat)));
    }

    ostream->active_axfers--;

    if (stat != USBD_CANCELLED && !ostream->active_axfers && ostream->rem_axfer)
    {
        /* If completed last transfer let's start reminder */
        stat = start_rem_axfer(stream, NULL, NULL);

        DBG_IF(stat != USBD_IN_PROGRESS)
        {
            DBG_E(DHOST_AUDIO_STREAM,("AUDIO: Failed to start last packet for "
                "write stream (%p), error=%s\n", stream, usbd_errstr(stat)));
        }
        
        /* If last frame failed we have no way to notify application */
    }

    if (!out_axfer->cb)
    {
        free_audio_xfer(axfer);
        return;
    }

    STAILQ_INSERT_TAIL(&ostream->done_axfers, axfer, next);

    rc = jtask_schedule(stream->task, 0, out_stream_task, stream);
    if (rc)
    {
        DBG_E(DHOST_AUDIO_STREAM,("AUDIO: Stream (%p), can't start task\n", 
            stream));

        out_stream_task(stream);
    }
}

static juint16_t get_next_len(audio_out_stream_h ostream)
{
    juint16_t audio_frames = ostream->average.div;

    ostream->average.sum += ostream->average.rem;

    if (ostream->average.sum >= ostream->isoc_freq) 
    {
        ostream->average.sum -= ostream->isoc_freq;
        audio_frames++;
    }
    return audio_frames * ostream->audio_frame_size;
}

static juint32_t get_frames(audio_out_stream_h ostream, juint32_t buf_len)
{
#define JMAX_UINT32  0xffffffffUL 
#define MAX_ISOC_FREQ 8000

    if (buf_len >= JMAX_UINT32/MAX_ISOC_FREQ)
    {
        juint32_t div = ostream->sam_freq * ostream->audio_frame_size;
        juint32_t result;

        result = (buf_len / div) * ostream->isoc_freq + 
            (buf_len % div) * ostream->isoc_freq / div;
        
        return result;
    }
    else
    {
        return buf_len * ostream->isoc_freq /
            (ostream->sam_freq * ostream->audio_frame_size);
    }
}

static jresult_t audio_out_stream_write(audio_stream_h stream, void *buf, 
    juint32_t data_len, audio_stream_cb_t cb, void *arg)
{
    void *buffer;
    juint32_t len, i, frames;
    usbd_status stat = USBD_NORMAL_COMPLETION;
    jresult_t rc;
    audio_xfer_h axfer;
    audio_out_stream_h ostream = GET_OUT_STREAM(stream);

    KASSERT(stream, ("Stream handle is NULL\n"));
    KASSERT(buf || !data_len, ("Stream write buffer is NULL\n"));
    KASSERT(!STREAM_IS_IN(stream), ("Trying write to IN stream\n"));

    DBG_V(DHOST_AUDIO_STREAM, ("AUDIO: audio_out_stream_write %p, len=%d arg=%p"
        "\n", stream, data_len, arg));
    /* TODO implementaion for format II */

    if (data_len % ostream->audio_frame_size)
    {
        DBG_E(DHOST_AUDIO_STREAM,("AUDIO: audio_out_stream_write (%p) failed, "
            "data_len(%d) is not aligned to audio frame size(%d)\n", stream,
            data_len, ostream->audio_frame_size));
        return JEINVAL;
    }

    buffer = buf;
    len = data_len;

    if (ostream->rem_axfer)
        start_rem_axfer(stream, &buffer, &len);

    frames = get_frames(ostream, len);
    axfer = alloc_audio_xfer(stream, frames, 0);
    if (!axfer)
    {
        DBG_E(DHOST_AUDIO_STREAM,("AUDIO: audio_out_stream_write (%p) failed, "
            "no memory for xfer\n", stream));
        return JENOMEM;
    }

    /* XXX virtual and physical addresses are equal on this branch */
    usbd_setup_isoc_xfer(axfer->xfer, stream->pipe, axfer, 
        axfer->fr_len, frames, USBD_DMA_BUF, out_axfer_complete);
    
    for (i = 0; i < frames; i++)
    {
        len -= axfer->fr_len[i] = get_next_len(ostream);
        axfer->xfer->length += axfer->fr_len[i];
    }

    axfer->xfer->buffer = buffer;
    
    GET_OUT_AXFER(axfer)->cb = cb;
    GET_OUT_AXFER(axfer)->arg = arg;

    if (len)
    {
        juint16_t fr_len = get_next_len(ostream);

        ostream->rem_axfer = alloc_audio_xfer(stream, 1, fr_len);
        if (!ostream->rem_axfer)
        {
            rc = JENOMEM;
            goto Error;            
        }

        j_memcpy(usbd_get_buffer(ostream->rem_axfer->xfer), 
            (juint8_t *)buf + data_len - len, len);

        *ostream->rem_axfer->fr_len = fr_len;
        ostream->rem_axfer_len = len;
    }

    ostream->active_axfers++;
    
    stat = usbd_transfer(axfer->xfer);
    
    rc = usbd_status_to_result(stat);

    if (rc)
        goto Error;

    DBG_X(DHOST_AUDIO_STREAM, ("AUDIO: audio_out_stream_write - %p, frames=%d "
        "rem=%d\n", stream, frames, len));

    return 0;

Error:
    if (axfer)
        free_audio_xfer(axfer);

    if (ostream->rem_axfer)
    {
        free_audio_xfer(ostream->rem_axfer);
        ostream->rem_axfer = NULL;
    }

    DBG_E(DHOST_AUDIO_STREAM,("AUDIO: audio_out_stream_write (%p) failed, "
        "error - %s\n", stream, usbd_errstr(stat)));

    return rc;
}

/**
 * Function name:  get_isoc_xfer_len
 * Description:    Calculate isochronous transfer actual length
 * Parameters:
 *     @fr_len: (IN) frames length array
 *     @fr_num: (IN) frames number
 *
 * Return value:   transfer length
 * Scope:          Local
 **/
static juint32_t get_isoc_xfer_len(juint16_t *fr_len, juint32_t fr_num)
{
    juint32_t sum = 0;

    while (fr_num--)
        sum += *(fr_len++);

    return sum;
}

/**
 * Function name:  audio_in_stream_get_data_size
 * Description:    Returns received data length in stream buffer
 * Parameters:
 *     @stream: (IN) Stream handle.
 *
 * Return value:   data length
 * Scope:          Local
 **/
static juint32_t audio_in_stream_get_data_size(audio_stream_h stream)
{
    audio_xfer_h axfer;
    juint32_t len = 0, frame_len;
    jbool_t started = FALSE;
    audio_in_stream_h istream = GET_IN_STREAM(stream);

    KASSERT(stream, ("Stream handle is NULL\n"));
    KASSERT(STREAM_IS_IN(stream), ("Can't be used for output stream!\n"));

    STAILQ_FOREACH(axfer, &istream->axfers.done, next)
    {
        if (axfer == STAILQ_FIRST(&istream->axfers.done))
        {
            frame_len = get_isoc_xfer_len(axfer->fr_len + istream->done_frame, 
                istream->frames_in_buf - istream->done_frame) - 
                istream->done_in_frame;
        }
        else
        {
            frame_len = get_isoc_xfer_len(axfer->fr_len,istream->frames_in_buf);
        }

        len += frame_len;

        if (!(stream->flags & AUDIO_STREAM_USE_DELIMITER))
            continue;

        if (started && !frame_len)
            goto Exit;

        if (frame_len)
            started = TRUE;
    }

Exit: 
    DBG_X(DHOST_AUDIO_STREAM, ("AUDIO: audio_in_stream_get_data_size - %p, "
        "len=%d\n", stream, len));
    return len;
}

/**
 * Function name:  audio_in_stream_read
 * Description:    Read data from stream buffer
 * Parameters:
 *     @stream: (IN)  Stream handle.
 *     @buf:    (OUT) User buffer.
 *     @len:    (IN/OUT) Buffer length / data length
 *
 * Return value:   None
 * Scope:          Local
 **/
static void audio_in_stream_read(audio_stream_h stream, void *buf, 
    juint32_t *len)
{
    audio_xfer_h axfer;
    juint32_t to_read;
    juint8_t *dest = (juint8_t *)buf;
    juint16_t max_packet;
    jresult_t rc;
    audio_in_stream_h istream = GET_IN_STREAM(stream);

    KASSERT(stream, ("Stream handle is NULL\n"));
    KASSERT(len, ("len arg is NULL\n"));
    KASSERT(buf, ("buf arg is NULL\n"));

    DBG_X(DHOST_AUDIO_STREAM, ("AUDIO: audio_in_stream_read - %p, buf=%p "
        "len=%d\n", stream, buf, *len));
    
    to_read = *len;
    max_packet = PIPE_GET_PACKET_LEN(stream->pipe);

    while ((axfer = STAILQ_FIRST(&istream->axfers.done)) != NULL && to_read)
    {
        juint16_t packet_len = axfer->fr_len[istream->done_frame];
        juint16_t data_len = MIN(to_read, packet_len - istream->done_in_frame);

        juint8_t *source = (juint8_t *)usbd_get_buffer(axfer->xfer);
        source += istream->done_frame * max_packet + istream->done_in_frame;

        j_memcpy(dest, source, data_len);

        dest += data_len;
        to_read -= data_len;

        istream->done_in_frame += data_len;
        if (istream->done_in_frame == packet_len)
        {
            istream->done_in_frame = 0;
            istream->done_frame++;
            if (istream->done_frame == istream->frames_in_buf)
            {
                istream->done_frame = 0;
                STAILQ_REMOVE_HEAD(&istream->axfers.done, next);
                STAILQ_INSERT_TAIL(&istream->axfers.free, axfer, next);
            }
        }
    }

    *len -= to_read;
    
    DBG_X(DHOST_AUDIO_STREAM, ("AUDIO: audio_in_stream_read - %p, act_len=%d\n",
        stream, *len));

    if (STAILQ_FIRST(&istream->axfers.free))
    {
        rc = jtask_schedule(stream->task, 0, in_stream_task, stream);
        DBG_IF(rc)
        {
            DBG_W(DHOST_AUDIO_STREAM, ("AUDIO: audio_in_stream_read - %p, "
                "warning: can't start task\n"));
        }
    }
}
/* Streaming end; additional streaming functions found later on. */

/**
 * Function name:  uwh_audio_samp_freq_to_uint32
 * Description:    Converts sampling frequency from a 3-bytes format to Hz.
 * Parameters:
 *     @freq: (IN) Sampling frequency expressed in a 3-bytes format.
 *
 * Return value:   Frequency in units of Hz.
 * Scope:          Global
 **/
juint32_t uwh_audio_samp_freq_to_uint32(uwh_audio_samp_freq_t *freq)
{
    juint32_t f = 0;

    j_memcpy(&f, freq, 3);

    return le32toh(f);
}

/**
 * Function name:  uwh_audio_uint32_to_samp_freq
 * Description:    Converts sampling frequency from Hz to a 3-bytes format.
 * Parameters:
 *     @freq_hz:  (IN)  Sampling frequency in Hz.
 *     @freq:     (OUT) Pointer to a 3-bytes format presentation of the given
 *                      sampling frequency (freq_hz).
 *
 * Return value:   None.
 * Scope:          Global
 **/
void uwh_audio_uint32_to_samp_freq(juint32_t freq_hz, 
    uwh_audio_samp_freq_t *freq)
{
    freq_hz = htole32(freq_hz);
    j_memcpy(freq, &freq_hz, 3);
}

/**
 * Function name:  uwh_audio_get_first_entity
 * Description:    Returns the first audio entity in the Audio Class Driver's
 *                 device entities list.
 * Parameters:
 *     @drv_ctx:   (IN) Audio driver context.
 *
 * Return value:   Pointer to the first audio entity in the list.
 * Scope:          Global
 **/
uwh_audio_entity_t *uwh_audio_get_first_entity(uwh_audio_drv_h drv_ctx)
{
    jaudio_softc_t *sc = (jaudio_softc_t *)drv_ctx;

    KASSERT(drv_ctx, ("AUDIO: driver context is NULL\n"));

    return GET_ENTITY(sc->units);
}

/**
 * Function name:  uwh_audio_get_next_entity
 * Description:    Returns the next audio entity in the Audio Class Driver's
 *                 device entities list.
 * Parameters:
 *     @drv_ctx:    (IN) Audio driver context.
 *     @cur_entity: (IN) Pointer to the current audio entity in the class
 *                       driver's device entities list.
 *
 * Return value:   Pointer to next audio entity in the list, or NULL if the
 *                 current entity is the last entity in the list.
 * Scope:          Global
 **/
uwh_audio_entity_t *uwh_audio_get_next_entity(uwh_audio_drv_h drv_ctx, 
    uwh_audio_entity_t *cur_entity)
{
    KASSERT(drv_ctx, ("AUDIO: driver context is NULL\n"));

    if (!cur_entity)
        return uwh_audio_get_first_entity(drv_ctx);

    return GET_ENTITY(GET_UNIT(cur_entity)->next);
}

/**
 * Function name:  get_unit
 * Description:    Returns a unit by entity ID.
 * Parameters:
 *     @sc: (IN) Audio driver context.
 *     @id: (IN) Audio entity ID.
 *
 * Return value:   Pointer to unit, or NULL if no matching unit is found.
 * Scope:          Local
 **/
static audio_unit_t *get_unit(jaudio_softc_t *sc, juint8_t id)
{
    KASSERT(sc->units_map, ("Units map was not created\n"));
    if (id > sc->unit_max_id)
        return NULL;

    return sc->units_map[id];
}

/**
 * Function name:  uwh_audio_get_entity
 * Description:    Returns an audio entity by ID.
 * Parameters:
 *     @drv_ctx:   (IN) Audio driver context.
 *     @entity_id: (IN) Audio entity ID.
 *
 * Return value:   Pointer to entity, or NULL if no matching entity is found.
 * Scope:          Global
 **/
uwh_audio_entity_t *uwh_audio_get_entity(uwh_audio_drv_h drv_ctx, 
    juint8_t entity_id)
{
    jaudio_softc_t *sc = (jaudio_softc_t *)drv_ctx;
    
    KASSERT(drv_ctx, ("AUDIO: driver context is NULL\n"));
    
    return GET_ENTITY(get_unit(sc, entity_id));
}

/**
 * Function name:  uwh_audio_ctrl_req
 * Description:    Sends a control request to the audio device.
 * Parameters:
 *     @drv_ctx: (IN)     Audio driver context 
 *     @req:     (IN/OUT) Pointer to an audio control request structure.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Global
 **/
jresult_t uwh_audio_ctrl_req(uwh_audio_drv_h drv_ctx, uwh_audio_ctrl_req_t *req)
{
    jaudio_softc_t *sc = (jaudio_softc_t *)drv_ctx;
    usb_device_request_t dreq;
    audio_unit_t *unit = GET_UNIT(req->entity);
    generic_terminal_t *trm;
    usbd_status stat;
    jresult_t rc;

    KASSERT(drv_ctx, ("AUDIO: driver context is NULL\n"));

    if (sc->flags & AUDIO_DETACH)
        return JEBUSY;

    if (!req->entity)
    {
        DBG_E(DHOST_AUDIO,("AUDIO: uwh_audio_ctrl_req, entity is NULL\n"));
        return JEINVAL;
    }

    DBG_V(DHOST_AUDIO, ("AUDIO: uwh_audio_ctrl_req, entity(%d) "
        "code(%02x) selector(%02x)\n", req->entity->entity_id, req->code, 
        req->ctrl_sel));

    dreq.bRequest = req->code;
    USETW(dreq.wLength, req->buf_len);

    switch (req->type)
    {
    case UWH_AR_ENDPOINT:

        dreq.bmRequestType = (req->code & UT_READ) | UT_CLASS | UT_ENDPOINT;

        trm = (generic_terminal_t *)req->entity->u.in_trm;

        if (!trm->sif || !trm->sif[0].ep_desc)
            return JENOTSUP;

        if (trm->sif->ep_audio_desc && req->code == UWH_ARC_SET_CUR)
        {
            if (!(trm->sif->ep_audio_desc->bmAttributes & 
                (1 << (req->ctrl_sel - 1))))
            {
                return JENOTSUP;
            }
        }

        USETW(dreq.wIndex, trm->sif[0].ep_desc->bEndpointAddress);
        USETW(dreq.wValue, (juint16_t)req->ctrl_sel << 8);
        break;

    case UWH_AR_CTRL_INTERFACE:
        
        dreq.bmRequestType = (req->code & UT_READ) | UT_CLASS | UT_INTERFACE;

        USETW(dreq.wIndex, ((juint16_t)req->entity->entity_id << 8) |
            sc->iface->idesc->bInterfaceNumber);

        USETW(dreq.wValue, ((juint16_t)req->ctrl_sel << 8) | req->value);
        break;

    case UWH_AR_STREAM_INTERFACE:
        
        dreq.bmRequestType = (req->code & UT_READ) | UT_CLASS | UT_INTERFACE;

        USETW(dreq.wIndex, ((juint16_t)req->entity->entity_id << 8) |
            unit->stream_iface);

        USETW(dreq.wValue, ((juint16_t)req->ctrl_sel << 8) | req->value);
        break;
    }

    DBG_V(DHOST_AUDIO, ("uwh_audio_ctrl_req bmReqType(%02x) bReq(%02x) "
        "wValue(%04x) wIndex(%04x) len=%d\n", dreq.bmRequestType, dreq.bRequest,
        UGETW(dreq.wValue), UGETW(dreq.wIndex), UGETW(dreq.wLength)));

    stat = usbd_do_request(sc->dev, &dreq, req->buffer);
    rc = usbd_status_to_result(stat);

    DBG_IF(rc)
        DBG_E(DHOST_AUDIO,("AUDIO: uwh_audio_ctrl_req failed, error=%d\n", rc));

    if (!rc && req->type == UWH_AR_ENDPOINT && 
        req->ctrl_sel == UWH_AEP_SAMPLING_FREQ_CONTROL)
    {
        if (req->code == UWH_ARC_GET_CUR)
        {
            unit->sam_freq = uwh_audio_samp_freq_to_uint32(req->buffer);

            DBG_V(DHOST_AUDIO, ("AUDIO: uwh_audio_ctrl_req, entity(%d) "
                "SAMPLING_FREQ_CONTROL %d Hz\n", req->entity->entity_id, 
                unit->sam_freq));
        }
        else if (req->code == UWH_ARC_SET_CUR)
        {
            rc = uwh_audio_samp_freq_req(drv_ctx, req->entity, UWH_ARC_GET_CUR,
                &unit->sam_freq);
        }
    }

    return rc;
}

jresult_t uwh_audio_samp_freq_req(uwh_audio_drv_h drv_ctx, 
    uwh_audio_entity_t *trm, uwh_audio_req_code_t code, juint32_t *freq_hz)
{
    jaudio_softc_t *sc = (jaudio_softc_t *)drv_ctx;
    uwh_audio_ctrl_req_t req;
    uwh_audio_samp_freq_t freq;
    jresult_t rc;

    KASSERT(drv_ctx, ("AUDIO: uwh_audio_samp_freq_req - drv_ctx is NULL\n"));
    KASSERT(trm,     ("AUDIO: uwh_audio_samp_freq_req - trm is NULL\n"));
    KASSERT(freq_hz, ("AUDIO: uwh_audio_samp_freq_req - freq_hz is NULL\n"));

    if (sc->flags & AUDIO_DETACH)
        return JEBUSY;

    DBG_X(DHOST_AUDIO, ("AUDIO: uwh_audio_samp_freq_req, trm=%d freq=%d Hz\n",
        trm->entity_id, *freq_hz));

    /* Init audio control request */
    req.entity = trm;

    req.type = UWH_AR_ENDPOINT;
    req.code = code;
    req.ctrl_sel = UWH_AEP_SAMPLING_FREQ_CONTROL;
    req.value = 0; /* unused for sampling frequency control */

    if (!(code & UT_READ))
        uwh_audio_uint32_to_samp_freq(*freq_hz, &freq);

    req.buffer  = freq;
    req.buf_len = sizeof(uwh_audio_samp_freq_t);

    rc = uwh_audio_ctrl_req(drv_ctx, &req);
    if (rc)
        return rc;

    if (code & UT_READ)
        *freq_hz = uwh_audio_samp_freq_to_uint32(&freq);

    return 0;
}

jresult_t uwh_audio_set_stream(uwh_audio_drv_h drv_ctx, uwh_audio_entity_t *trm, 
    uwh_audio_stream_iface_t *sif)
{
    jaudio_softc_t *sc = (jaudio_softc_t *)drv_ctx;
    audio_unit_t *unit = GET_UNIT(trm);
    usbd_status stat;
    jresult_t rc;
    juint8_t alt = 0;

    KASSERT(drv_ctx, ("AUDIO: driver context is NULL\n"));

    if (sc->flags & AUDIO_DETACH)
        return JEBUSY;

    if (!unit || !unit->stream_iface)
    {
        DBG_E(DHOST_AUDIO_STREAM,("AUDIO: uwh_audio_set_stream failed, wrong "
            "terminal\n"));
        return JEINVAL;
    }
    
    if (sif)
        alt = sif - trm->u.out_trm->sif + 1;

    DBG_I(DHOST_AUDIO_STREAM, ("AUDIO: uwh_audio_set_stream terminal(%d) "
        "iface(%d) alt_set(%d)\n", trm->entity_id, unit->stream_iface, alt));

    if (unit->stream)
        audio_stream_close(unit->stream);

    unit->stream = NULL;

    stat = usbd_set_interface(&sc->dev->ifaces[unit->stream_iface], alt);
    if (stat != USBD_NORMAL_COMPLETION)
    {
        DBG_E(DHOST_AUDIO,("AUDIO: uwh_audio_set_stream failed, error - %s\n", 
            usbd_errstr(stat)));
        return usbd_status_to_result(stat);
    }

    rc = uwh_audio_samp_freq_req(drv_ctx, trm, UWH_ARC_GET_CUR, 
        &unit->sam_freq);
    if (rc)
    {
        DBG_E(DHOST_AUDIO,("AUDIO: uwh_audio_set_stream trm(%d) "
            "failed, can't read current sampling frequency, rc=%d\n", 
            trm->entity_id, rc));
    }

    return rc;
}

static jresult_t audio_write_open(uwh_audio_entity_t *entity)
{
    jresult_t rc = 0;
    audio_unit_t *unit = GET_UNIT(entity);
    usbd_interface_handle iface;
    juint8_t sif_num, alt_index, ep_addr;
    juint8_t flags;
    juint16_t audio_frame_size = 0;
    uwh_audio_stream_iface_t *sif;

    DBG_V(DHOST_AUDIO_STREAM, ("AUDIO: audio_write_open, trm(%d)\n", 
        entity->entity_id));

    iface = &unit->sc->dev->ifaces[unit->stream_iface];
    alt_index = usbd_get_interface_altindex(iface);

    if (!alt_index)
    {
        DBG_E(DHOST_AUDIO_STREAM,("AUDIO: audio_write_open trm(%d) failed, "
            "no streaming alt setting selected\n", entity->entity_id));
        rc = JEINVAL;
        goto Exit;
    }

    sif_num = alt_index - 1;
    sif = &entity->u.in_trm->sif[sif_num];
    ep_addr = sif->ep_desc->bEndpointAddress;

    flags = AUDIO_STREAM_IGNORE_EIO;

    if (entity->u.in_trm->sif[sif_num].format_type == UWH_AF_TYPE_II)
    {
        flags |= AUDIO_STREAM_USE_DELIMITER;
    }
    else
    {
        audio_frame_size = sif->format.type_I->bNrChannels * 
            sif->format.type_I->bSubframeSize;
    }

    rc = audio_out_stream_open(unit->sc->dev, iface, ep_addr, audio_frame_size,
        unit->sam_freq, flags, &unit->stream);
Exit:
    return rc;
}

/**
 * Function name:  uwh_audio_write_data
 * Description:    Writes data to the device using the given USB audio stream.
 * Parameters:
 *     @entity:  (IN) Pointer to a USB streaming terminal entity structure.
 *     @buf:     (IN) Handle to a memory buffer containing the data to write.
 *     @len:     (IN) Data length, in bytes.
 *     @cb:      (IN) Write completion application callback:
 *                      typedef void (*uwh_audio_write_cb_t)(void *arg,
 *                          jresult_t status);
 *                      Parameters:
 *                          @arg:   (IN) Pointer to callback arguments.
 *                          @status (IN) Write transfer status.
 *                      Return Value: None.
 *     @arg:     (IN) Pointer to arguments for the application's write
 *                    completion callback (cb).
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Global
 **/
jresult_t uwh_audio_write_data(uwh_audio_entity_t *entity, void *buf, 
    juint32_t len, uwh_audio_write_cb_t cb, void *arg)
{
    jresult_t rc = 0;
    audio_unit_t *unit = GET_UNIT(entity);

    KASSERT(entity, ("AUDIO: entity is NULL\n"));

    DBG_V(DHOST_AUDIO_STREAM, ("AUDIO: uwh_audio_write_data, trm(%d) len=%d\n", 
        entity->entity_id, len));

    if (unit->sc->flags & AUDIO_DETACH)
        return JEBUSY;

    if (!unit->stream)
    {
        rc = audio_write_open(entity);
        if (rc)
            return rc;
    }

    return audio_out_stream_write(unit->stream, buf, len, (audio_stream_cb_t)cb,
        arg);
}

/**
 * Function name:  uwh_audio_read_start
 * Description:    Starts reading data from the device using the given USB
 *                 audio stream.
 *                 The size of the read stream buffer is determined by the
 *                 buf_time_ms parameter. When the data is received from the
 *                 device, the application's read callback function (cb) is
 *                 called. If the application does not call
 *                 uwh_audio_read_get_data() to retrieve the read data within
 *                 buf_time_ms of the callback's arrival, the next read attempt
 *                 using the same stream buffer will fail, the read stream will
 *                 be stopped (aborted), and the application's read callback
 *                 will be called with an JETIMEDOUT status.
 * Parameters:
 *     @entity:      (IN) Pointer to a USB streaming terminal entity structure.
 *     @buf_time_ms: (IN) Stream buffering time, in milliseconds.
 *     @cb:          (IN) Application read callback:
 *                          typedef void (*uwh_audio_read_cb_t)(void *arg,
 *                              jresult_t status);
 *                        Parameters:
 *                          @arg:   (IN) Pointer to callback arguments.
 *                          @status (IN) Read stream status.
 *                        Return Value: None.
 *     @arg:         (IN) Pointer to arguments for the application's read
 *                        callback (cb).
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Global
 **/
jresult_t uwh_audio_read_start(uwh_audio_entity_t *entity, juint16_t buf_time_ms, 
    uwh_audio_read_cb_t cb, void *arg)
{
    jresult_t rc = 0;
    audio_unit_t *unit = GET_UNIT(entity);
    usbd_interface_handle iface;
    juint8_t sif_num, alt_index, ep_addr;
    juint32_t buf_time_fr;
    juint8_t flags;

    KASSERT(entity, ("AUDIO: entity handle is NULL\n"));

    DBG_V(DHOST_AUDIO_STREAM, ("AUDIO: uwh_audio_read_start, trm(%d) "
        "buf_time=%d ms\n", entity->entity_id, buf_time_ms));

    if (unit->sc->flags & AUDIO_DETACH)
        return JEBUSY;

    if (!buf_time_ms)
        return JEINVAL;

    iface = &unit->sc->dev->ifaces[unit->stream_iface];
    alt_index = usbd_get_interface_altindex(iface);

    if (!alt_index)
    {
        DBG_E(DHOST_AUDIO,("AUDIO: No streaming alt setting selected\n"));
        rc = JEINVAL;
        goto Exit;
    }

    sif_num = alt_index - 1;
    ep_addr = entity->u.out_trm->sif[sif_num].ep_desc->bEndpointAddress;

    if (unit->sc->dev->speed == USB_SPEED_HIGH)
        buf_time_fr = 8 * buf_time_ms;
    else
        buf_time_fr = buf_time_ms;

    flags = AUDIO_STREAM_IGNORE_EIO;
    if (entity->u.out_trm->sif[sif_num].format_type == UWH_AF_TYPE_II)
        flags |= AUDIO_STREAM_USE_DELIMITER;

    rc = audio_in_stream_open(unit->sc->dev, iface, ep_addr, buf_time_fr, flags,
        &unit->stream);
    if (rc)
        goto Exit;

    rc = audio_in_stream_start(unit->stream, (audio_stream_cb_t)cb, arg);
Exit:
    return rc;
}

/**
 * Function name:  uwh_audio_read_get_data_size
 * Description:    Returns the actual amount of data received from the device
 *                 and not yet read by the application.
 *                 This function is used for read transfers of fixed-size audio
 *                 formats - i.e. Type I and Type III audio data format types.
 * Parameters:
 *     @entity: (IN) Pointer to a USB streaming terminal entity structure.
 *
 * Return value:   Read data size, in bytes.
 * Scope:          Global
 **/
juint32_t uwh_audio_read_get_data_size(uwh_audio_entity_t *entity)
{
    audio_stream_h stream;

    KASSERT(entity, ("AUDIO: Entity handle is NULL\n"));

    stream = GET_UNIT(entity)->stream;

    if (!stream)
    {
        DBG_E(DHOST_AUDIO,("AUDIO: Stream was not opened\n"));
        return 0;
    }

    return audio_in_stream_get_data_size(stream);
}

/**
 * Function name:  uwh_audio_read_get_frame_size
 * Description:    Returns the size of a given USB audio stream's next read
 *                 frame available for the application.
 *                 This function is used for audio formats with non-fixed frame
 *                 sizes - i.e. Type II audio data format types.
 * Parameters:
 *     @entity: (IN) Pointer to a USB streaming terminal entity structure.
 *
 * Return value:   Frame size, in bytes.
 * Scope:          Global
 **/
/* Used for type II format */
juint32_t uwh_audio_read_get_frame_size(uwh_audio_entity_t *entity)
{
    KASSERT(entity, ("AUDIO: Entity handle is NULL\n"));

    return uwh_audio_read_get_data_size(entity);
}

/**
 * Function name:  uwh_audio_read_get_data
 * Description:    Reads data from the read buffer of a given USB audio stream.
 * Parameters:
 *     @entity:  (IN) Pointer to a USB streaming terminal entity structure.
 *     @buffer: (OUT) Handle to a pre-allocated memory buffer to be filled with
 *                    the read data.
 *     @size:   (IN)  The size of the provided data buffer, in bytes.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Global
 **/
jresult_t uwh_audio_read_get_data(uwh_audio_entity_t *entity, 
    void *buffer, juint32_t size)
{
    audio_stream_h stream;
    juint32_t act_size;

    KASSERT(entity, ("AUDIO: Entity handle is NULL\n"));

    stream = GET_UNIT(entity)->stream;

    if (!stream)
    {
        DBG_E(DHOST_AUDIO,("AUDIO: Stream was not opened\n"));
        return JEINVAL;
    }

    act_size = audio_in_stream_get_data_size(stream);
    if (act_size < size)
    {
        DBG_E(DHOST_AUDIO,("AUDIO: Not enough data, in stream - %d bytes "
            "(requested - %d)\n", act_size, size));
        return JEINVAL;
    }

    audio_in_stream_read(stream, buffer, &size);
    return 0;
}

/**
 * Function name:  uwh_audio_stream_abort
 * Description:    Aborts all transfers for the given USB audio stream and
 *                 closes the stream.
 * Parameters:
 *     @drv_ctx: (IN) Audio driver context.
 *     @entity:  (IN) Pointer to a USB streaming terminal entity structure.
 *
 * Return value:   None
 * Scope:          Global
 **/
void uwh_audio_stream_abort(uwh_audio_drv_h drv_ctx, uwh_audio_entity_t *entity)
{
    audio_stream_h stream;

    KASSERT(entity, ("AUDIO: Entity handle is NULL\n"));

    stream = GET_UNIT(entity)->stream;

    if (!stream)
        return;

    audio_stream_close(stream);

    GET_UNIT(entity)->stream = NULL;
}

/**
 * Function name:  get_next_generic_desc
 * Description:    Returns the next descriptor in the configuration.
 * Parameters:
 *     @cur_desc:  (IN) Current descriptor.
 *     @start:     (IN) Pointer to the first descriptor in the configuration.
 *     @total_len: (IN) Total descriptors length, in bytes.
 *
 * Return value:   Pointer to the next descriptor in the configuration, or NULL
 *                 if the current descriptor (cur_desc) is the last descriptor.
 * Scope:          Local
 **/
static usb_descriptor_t *get_next_generic_desc(void *cur_desc, void *start, 
    juint16_t total_len)
{
    juint8_t  desc_len = ((usb_descriptor_t *)cur_desc)->bLength;
    juint16_t desc_offset = (juint8_t *)cur_desc - (juint8_t *)start;

    if (desc_len + desc_offset >= total_len)
        return NULL;
    
    return (usb_descriptor_t *)((juint8_t *)cur_desc + desc_len);
}

/**
 * Function name:  get_next_ac_desc
 * Description:    Returns the next descriptor in an Audio Control interface.
 * Parameters:
 *     @sc:       (IN) Audio driver context.
 *     @cur_desc: (IN) Current descriptor.
 *
 * Return value:   Pointer to the next descriptor in the Audio Control
 *                 interface, or NULL if the current descriptor (cur_desc) is
 *                 the last descriptor in the interface.
 * Scope:          Local
 **/
static usb_descriptor_t *get_next_ac_desc(jaudio_softc_t *sc, void *cur_desc)
{
    return (usb_descriptor_t *)(get_next_generic_desc(cur_desc, 
        /* HSU addition */(void*)/* HSU addition */sc->header_desc, UGETW(sc->header_desc->wTotalLength)));
}

/**
 * Function name:  get_next_desc
 * Description:    Returns the next descriptor in the active configuration.
 * Parameters:
 *     @sc:       (IN) Audio driver context.
 *     @cur_desc: (IN) Current descriptor.
 *
 * Return value:   Pointer to the next descriptor in the active configuration,
 *                 or NULL if the current descriptor (cur_desc) is the last
 *                 descriptor.
 * Scope:          Local
 **/
static usb_descriptor_t *get_next_desc(jaudio_softc_t *sc, void *cur_desc)
{
    usb_config_descriptor_t *conf = usbd_get_config_descriptor(sc->dev);
    
    return (usb_descriptor_t *)(get_next_generic_desc(cur_desc, /* HSU addition */(void*)/* HSU addition */conf, 
        UGETW(conf->wTotalLength)));
}

/**
 * Function name:  generic_unit_init
 * Description:    Initializes a generic unit.
 * Parameters:
 *     @sc:       (IN) Audio driver context.
 *     @unit:     (IN) Pointer to an audio unit.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t generic_unit_init(jaudio_softc_t *sc, audio_unit_t *unit)
{
    DBG_V(DHOST_AUDIO, ("AUDIO: generic_unit_init, id=%d\n",
        unit->entity.entity_id));

    unit->entity.u.unit_desc = /* HSU addition */(void*)/* HSU addition */unit->desc;

    return 0;
}

/**
 * Function name:  streaming_trm_init
 * Description:    Creates and initializes streaming interfaces for terminals.
 *                 Used in in_terminal_init() and out_terminal_init().
 * Parameters:
 *     @sc:               (IN)  Audio driver context.
 *     @unit:             (IN)  Pointer to an audio unit.
 *     @stream_iface:     (OUT) Pointer to the streaming interface array created
 *                              by the function.
 *     @stream_iface_num: (OUT) The number of interfaces in the stream_iface
 *                              array.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t streaming_trm_init(jaudio_softc_t *sc, audio_unit_t *unit,
    uwh_audio_stream_iface_t **stream_iface, juint8_t *stream_iface_num)
{
    jresult_t rc = 0;
    uwh_audio_stream_iface_t *sif = NULL;
    juint_t i;
    juint8_t sif_num;
    usb_descriptor_t *desc;
    
    DBG_V(DHOST_AUDIO, ("AUDIO: streaming_trm_init (%d)\n", 
        unit->entity.entity_id));

    if (!unit->stream_iface)
    {
        DBG_E(DHOST_AUDIO, ("AUDIO: streaming_trm_init (%d), no streaming "
            "interface found\n", unit->entity.entity_id));
        rc = JEINVAL;
        goto Error;
    }

    sif_num = usbd_get_no_alts(sc->dev->cdesc, unit->stream_iface);
    if (!sif_num)
    {
        DBG_E(DHOST_AUDIO, ("AUDIO: streaming_trm_init (%d), no interface\n",
            unit->entity.entity_id));
        rc = JENOENT;
        goto Error;
    }

    sif_num--;
    if (!sif_num)
    {
        DBG_W(DHOST_AUDIO, ("AUDIO: streaming_trm_init (%d),no alt setting\n",
            unit->entity.entity_id));
        goto Exit;
    }

    sif = (uwh_audio_stream_iface_t *)jmalloc(sif_num * 
        sizeof(uwh_audio_stream_iface_t), M_ZERO);
    if (!sif)
    {
        DBG_E(DHOST_AUDIO,("AUDIO: streaming_trm_init (%d), no memory\n", 
            unit->entity.entity_id));
        rc = JENOMEM;
        goto Error;
    }

    desc = (usb_descriptor_t *)sc->dev->ifaces[unit->stream_iface].idesc;

    /* Skip alt setting 0 */
    desc = get_next_desc(sc, /* HSU addition */(void*)/* HSU addition */desc);

    if (!desc || desc->bDescriptorType != UDESC_INTERFACE)
    {
        DBG_E(DHOST_AUDIO,("AUDIO: streaming_trm_init (%d), where is interface "
            "descriptor?\n", unit->entity.entity_id));
        rc = JENOENT;
        goto Error;
    }

    for (i = 0; i < sif_num; i++)
    {
        for (desc = get_next_desc(sc, /* HSU addition */(void*)/* HSU addition */desc); 
            desc && desc->bDescriptorType != UDESC_INTERFACE;
            desc = get_next_desc(sc, /* HSU addition */(void*)/* HSU addition */desc))
        {
            if (desc->bDescriptorType == UDESC_CS_INTERFACE)
            {
                switch (((usb_descriptor_t *)desc)->bDescriptorSubtype)
                {
                case UWH_ASD_GENERAL:
                    sif[i].sif_desc = (uwh_audio_sif_desc_t *)desc;
                    break;
                case UWH_ASD_FORMAT_TYPE:
                    sif[i].format.type_I = (uwh_audio_format_I_desc_t *)desc;
                    sif[i].format_type = (enum uwh_audio_format_type)
                        sif[i].format.type_I->bFormatType;
                    break;
                case UWH_ASD_FORMAT_SPECIFIC:
                    /* Format specific descriptors */
                    break;
                default:
                    break;
                }
            }

            else if (desc->bDescriptorType == UDESC_ENDPOINT)
                sif[i].ep_desc = (usb_endpoint_descriptor_t *)desc;

            else if (desc->bDescriptorType == UDESC_CS_ENDPOINT)
                sif[i].ep_audio_desc = (uwh_audio_isoc_ep_desc_t *)desc;
        }
        if (!sif[i].sif_desc || !sif[i].format.type_I || !sif[i].ep_desc)
        {
            DBG_E(DHOST_AUDIO,("AUDIO: streaming_trm_init (%d) alt=%d, some of "
                "descriptors are missing (GENERAL, FORMAT_TYPE or ENDPOINT)\n",
                unit->entity.entity_id, i+1));
            rc = JENOENT;
            goto Error;
        }
        DBG_V(DHOST_AUDIO, ("AUDIO: Added streaming iface alt=%d\n", i+1));
    }

    *stream_iface_num = sif_num;
    *stream_iface = sif;

Exit:
    return 0;

Error:
    if (sif)
        jfree(sif);

    return rc;
}

/**
 * Function name:  out_terminal_uninit
 * Description:    Un-initializes an output terminal.
 * Parameters:
 *     @sc:   (IN) Audio driver context.
 *     @unit: (IN) Pointer to an output terminal unit.
 *
 * Return value:   None.
 * Scope:          Local
 **/
static void out_terminal_uninit(jaudio_softc_t *sc, audio_unit_t *unit)
{
    if (unit->stream)
        audio_stream_close(unit->stream);

    unit->stream = NULL;

    if (unit->entity.u.out_trm)
    {
        if (unit->entity.u.out_trm->sif)
            jfree(unit->entity.u.out_trm->sif);

        jfree(unit->entity.u.out_trm);
    }
}

/**
 * Function name:  out_terminal_init
 * Description:    Initializes an output terminal.
 * Parameters:
 *     @sc:   (IN) Audio driver context.
 *     @unit: (IN) Pointer to an output terminal unit.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t out_terminal_init(jaudio_softc_t *sc, audio_unit_t *unit)
{
    uwh_audio_output_terminal_t *trm;
    jresult_t rc = 0;

    DBG_V(DHOST_AUDIO, ("AUDIO: out_terminal_init (%d)\n", 
        unit->entity.entity_id));

    trm = (uwh_audio_output_terminal_t *)jmalloc(
        sizeof(uwh_audio_output_terminal_t), M_ZERO);
    if (!trm)
    {
        DBG_E(DHOST_AUDIO, ("AUDIO: out_terminal_init (%d), no memory\n",
            unit->entity.entity_id));
        rc = JENOMEM;
        goto Error;
    }
    
    unit->entity.u.out_trm = trm;
    trm->terminal_desc     = (uwh_audio_output_terminal_desc_t *)unit->desc;
    unit->uninit_func      = out_terminal_uninit;

    if (UGETW(trm->terminal_desc->wTerminalType) == UWH_ATT_USB_STREAMING)
    {
        rc = streaming_trm_init(sc, unit, &trm->sif, &trm->sif_num);
        if (rc)
            goto Error;
    }

    return 0;

Error:
    if (trm)
        jfree(trm);

    unit->entity.u.out_trm = NULL;
    return rc;
}

/**
 * Function name:  in_terminal_uninit
 * Description:    Un-initializes and input terminal.
 * Parameters:
 *     @sc:   (IN) Audio driver context.
 *     @unit: (IN) Pointer to an input terminal unit.
 *
 * Return value:   None.
 * Scope:          Local
 **/
static void in_terminal_uninit(jaudio_softc_t *sc, audio_unit_t *unit)
{
    if (unit->stream)
        audio_stream_close(unit->stream);

    unit->stream = NULL;

    if (unit->entity.u.in_trm)
    {
        if (unit->entity.u.in_trm->sif)
            jfree(unit->entity.u.in_trm->sif);

        jfree(unit->entity.u.in_trm);
    }
}

/**
 * Function name:  in_terminal_init
 * Description:    Initializes an input terminal.
 * Parameters:
 *     @sc:   (IN) Audio driver context.
 *     @unit: (IN) Pointer to an input terminal unit.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t in_terminal_init(jaudio_softc_t *sc, audio_unit_t *unit)
{
    uwh_audio_input_terminal_t *trm;
    jresult_t rc = 0;

    DBG_V(DHOST_AUDIO, ("AUDIO: in_terminal_init (%d)\n", 
        unit->entity.entity_id));

    trm = (uwh_audio_input_terminal_t *)jmalloc(
        sizeof(uwh_audio_input_terminal_t), M_ZERO);
    if (!trm)
    {
        DBG_E(DHOST_AUDIO, ("AUDIO: in_terminal_init (%d), no memory\n",
            unit->entity.entity_id));
        rc = JENOMEM;
        goto Error;
    }
    
    unit->entity.u.in_trm = trm;
    trm->terminal_desc     = (uwh_audio_input_terminal_desc_t *)unit->desc;
    unit->uninit_func      = in_terminal_uninit;

    if (UGETW(trm->terminal_desc->wTerminalType) == UWH_ATT_USB_STREAMING)
    {
        rc = streaming_trm_init(sc, unit, &trm->sif, &trm->sif_num);
        if (rc)
            goto Error;
    }

    return 0;

Error:
    if (trm)
        jfree(trm);
    
    unit->entity.u.in_trm = NULL;
    return rc;
}

/**
 * Function name:  units_map_create
 * Description:    Creates an array of pointers to units.
 * Parameters:
 *     @sc: (IN) Audio driver context.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t units_map_create(jaudio_softc_t *sc)
{
    audio_unit_t *unit;

    DBG_V(DHOST_AUDIO, ("AUDIO: units_map_create, max_id=%d\n", 
        sc->unit_max_id));

    if (!sc->units)
        return JEINVAL;

    sc->units_map = (audio_unit_t **)
        jmalloc((sc->unit_max_id+1) * sizeof(audio_unit_t *), M_ZERO);

    if (!sc->units_map)
        return JENOMEM;

    for (unit = sc->units; unit; unit = unit->next)
        sc->units_map[unit->entity.entity_id] = unit;

    return 0;
}

/**
 * Function name:  unit_create
 * Description:    Creates an audio unit.
 * Parameters:
 *     @sc:   (IN)  Audio driver context.
 *     @desc: (IN)  Pointer to an audio entity descriptor.
 *     @unit: (OUT) Pointer to the unit created by the function.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t unit_create(jaudio_softc_t *sc, usb_descriptor_t *desc, 
    audio_unit_t **unit)
{
    generic_entity_desc_t *ed = (generic_entity_desc_t *)desc;

    *unit = (audio_unit_t *)jmalloc(sizeof(audio_unit_t), M_ZERO);
    if (!*unit)
    {
        DBG_E(DHOST_AUDIO,("AUDIO: unit_create - no memory\n"));
        return JENOMEM;
    }

    (*unit)->entity.type      = (uwh_audio_entity_type_t)ed->bDescriptorSubtype;
    (*unit)->entity.entity_id = ed->bEntityID;
    (*unit)->desc             = ed;
    (*unit)->sc               = sc;

    DBG_V(DHOST_AUDIO, ("AUDIO: Created unit, type=0x%02x ID=%d\n", 
        (*unit)->entity.type, (*unit)->entity.entity_id));

    return 0;
}

/**
 * Function name:  audio_parse_ctrl_iface_desc
 * Description:    Parses an Audio Control Interface class-specific descriptor,
 *                 and creates units, a units list and units map.
 * Parameters:
 *     @sc:     (IN) Audio driver context.
 *     @header: (IN) Pointer to a class-specific audio header descriptor.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t audio_parse_ctrl_iface_desc(jaudio_softc_t *sc, void *header)
{
    usb_descriptor_t *desc;
    audio_unit_t *unit;
    audio_unit_t **unit_list = &sc->units;
    jresult_t rc = 0;

    DBG_V(DHOST_AUDIO, ("AUDIO: Create entities list\n"));

    /* Skip HEADER descriptor */
    for (desc = get_next_ac_desc(sc, header); desc;
        desc = get_next_ac_desc(sc, /* HSU addition */(void*)/* HSU addition */desc))
    {
        /* Class specific descriptors */
        if (desc->bDescriptorType == UDESC_CS_INTERFACE)
        {
            DBG_X(DHOST_AUDIO, ("AUDIO: Found entity, type=0x%02x\n", 
                desc->bDescriptorSubtype));

            rc = unit_create(sc, desc, &unit);
            if (rc)
                goto Exit;
            
            sc->unit_max_id = MAX(sc->unit_max_id, unit->entity.entity_id);

            /* Add to list */
            *unit_list = unit;
            unit_list = &unit->next;
        }
#ifdef JDEBUG
        else
        {
            DBG_W(DHOST_AUDIO,("AUDIO: Skip unknown desc, desc type=0x%02x\n",
                desc->bDescriptorType));
        }
#endif
    }

    rc = units_map_create(sc);

Exit:
    return rc;
}

/**
 * Function name:  link_streaming_iface_to_unit
 * Description:    Finds the matching unit for a specified streaming interface.
 * Parameters:
 *     @sc:     (IN) Audio driver context.
 *     @if_num: (IN) Streaming interface index.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t link_streaming_iface_to_unit(jaudio_softc_t *sc, juint8_t if_num)
{
    usbd_interface_handle  iface;
    uwh_audio_sif_desc_t       *sif_desc;
    audio_unit_t           *unit;
    usb_descriptor_t       *alt_set;

    DBG_X(DHOST_AUDIO, ("AUDIO: Found Streaming Interface, index=%d\n",if_num));

    if (if_num >= sc->dev->cdesc->bNumInterface)
        return JEINVAL;

    iface = &sc->dev->ifaces[if_num];

    /* Skip alternate setting 0 */
    alt_set = get_next_desc(sc, /* HSU addition */(void*)/* HSU addition */(iface->idesc));
    if (!alt_set)
        return JENOENT;

    /* Get class specific GENERAL descriptor */
    sif_desc = (uwh_audio_sif_desc_t *)get_next_desc(sc, /* HSU addition */(void*)/* HSU addition */alt_set);

    if (!sif_desc || sif_desc->bDescriptorType != UDESC_CS_INTERFACE ||
        sif_desc->bDescriptorSubtype != UWH_ASD_GENERAL)
    {
        DBG_E(DHOST_AUDIO,("AUDIO: Can't find class specific GENERAL decriptor "
            "in Audio Streaming Interface, iface=%d\n", if_num));
        return JEINVAL;
    }

    unit = get_unit(sc, sif_desc->bTerminalLink);
    if (!unit)
    {
        DBG_E(DHOST_AUDIO,("AUDIO: Can't find terminal - %d\n",
            sif_desc->bTerminalLink));
        return JEINVAL;
    }

    DBG_V(DHOST_AUDIO, ("AUDIO: Interface (%d) connected to terminal - %d\n",
        if_num, unit->entity.entity_id));
    
    unit->stream_iface = if_num;
    
    return 0;
}

/**
 * Function name:  units_init
 * Description:    Initializes units according to entity type.
 * Parameters:
 *     @sc: (IN) Audio driver context.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t units_init(jaudio_softc_t *sc)
{
    audio_unit_t *unit;
    jresult_t rc = 0;
    
    DBG_V(DHOST_AUDIO, ("AUDIO: units_init\n"));

    for (unit = sc->units; unit && !rc; unit = unit->next)
    {
        switch (unit->entity.type)
        {
        case UWH_ACD_INPUT_TERMINAL:
            rc = in_terminal_init(sc, unit);
            break;
        case UWH_ACD_OUTPUT_TERMINAL:
            rc = out_terminal_init(sc, unit);
            break;

        default:
            rc = generic_unit_init(sc, unit);
            break;
        }
    }
    
    return rc;
}

/**
 * Function name:  units_uninit
 * Description:    Un-initializes units.
 *                 The function frees the memory allocated for the units.
 * Parameters:
 *     @sc: (IN) Audio driver context.
 *
 * Return value:   None.
 * Scope:          Local
 **/
static void units_uninit(jaudio_softc_t *sc)
{
    audio_unit_t *dead_unit;

    DBG_V(DHOST_AUDIO, ("AUDIO: units_uninit\n"));

    if (sc->units_map)
        jfree(sc->units_map);

    sc->units_map = NULL;

    while (sc->units)
    {
        dead_unit = sc->units;
        sc->units = sc->units->next;
        
        if (dead_unit->uninit_func)
            dead_unit->uninit_func(sc, dead_unit);
        
        jfree(dead_unit);
    }
}

/**
 * Function name:  intr_complete
 * Description:    Interrupt pipe transfer completion callback.
 * Parameters:
 *     @xfer:   (IN) Transfer handle.
 *     @sc_h:   (IN) Audio driver context.
 *     @status: (IN) USBD status.
 *
 * Return value:   None.
 * Scope:          Local
 **/
static void intr_complete(usbd_xfer_handle xfer, usbd_private_handle sc_h, 
    usbd_status status)
{
    jaudio_softc_t *sc = (jaudio_softc_t *)sc_h;
    uwh_audio_notify_info_t info;

    KASSERT(sc, ("AUDIO: driver context is NULL\n"));
    KASSERT(xfer, ("AUDIO: xfer handle is NULL\n"));

    if (sc->flags & AUDIO_DETACH)
        return;

    if (status != USBD_NORMAL_COMPLETION)
    {
        DBG_W(DHOST_AUDIO, ("AUDIO: Interrupt transfer failed, error - %s\n",
            usbd_errstr(status)));
        return;
    }
    
    info.originator.ep           = sc->intr.buf[1];
    info.originator_type         = (uwh_audio_notify_originator_t)
        (sc->intr.buf[0] & 0xf);
    info.memory_contents_changed = 1 & (sc->intr.buf[0] >> 6);
    info.interrupt_pending       = 1 & (sc->intr.buf[0] >> 7);

    sc->cbs->notify(sc->app_ctx, &info);
}

/**
 * Function name:  jaudio_probe
 * Description:    Driver probe driver method callback.
 *                 This callback is called by the USB Core in order to match new
 *                 attached device with this class driver. 
 * Parameters:
 *     @self: (IN) Private device data.
 *
 * Return value:   Match level.
 * Scope:          Local
 **/
static jresult_t jaudio_probe(jdevice_t self)
{
    struct usb_attach_arg *uaa = j_device_get_ivars(self);
    usb_interface_descriptor_t *id;
    
    DBG_I(DHOST_AUDIO, ("AUDIO: jaudio_probe\n"));

    if (!uaa->iface)    /* we attach only to interfaces */
        return UMATCH_NONE;

     /* Check that this is a Audio device. */
    id = usbd_get_interface_descriptor(uaa->iface);

    if (id && id->bInterfaceClass == UICLASS_AUDIO && 
        id->bInterfaceSubClass == UISUBCLASS_AUDIOCONTROL)
    {
        if (!appmgr)
        {
            DBG_E(DHOST_AUDIO, ("AUDIO: jaudio_probe - no upper layer\n"));
            return UMATCH_NONE;
        }

        if (uaa->iface->idesc->bInterfaceProtocol)
        {
            DBG_E(DHOST_AUDIO, ("AUDIO: jaudio_attach - ADC v2.0 not supported"
                "\n"));
            return UMATCH_NONE;
        }

        return UMATCH_IFACECLASS;
    }

    return UMATCH_NONE;
}

/**
 * Function name:  jaudio_attach
 * Description:    Device attach driver method callback.
 *                 This callback is called by the USB Core after an attached
 *                 device has been successfully matched to the class driver.
 * Parameters:
 *     @self: (IN) Private device data.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t jaudio_attach(jdevice_t self)
{
    jaudio_softc_t *sc = (jaudio_softc_t *)j_device_get_softc(self);
    struct usb_attach_arg *uaa = j_device_get_ivars(self);
    audio_control_iface_desc_t *header;
    juint_t i;
    jresult_t rc;

    /* Ignore Audio Streaming interfaces */
    
    DBG_I(DHOST_AUDIO, ("AUDIO: jaudio_attach started\n"));

    sc->sc_dev = self;
    sc->iface  = uaa->iface;
    sc->dev    = uaa->device;

    /* Read Audio Control Interface class specific descriptor - HEADER */
    header = (audio_control_iface_desc_t *)get_next_desc(sc, /* HSU addition */(void*)/* HSU addition */(sc->iface->idesc));
    sc->header_desc = header;

    if (!header || header->bDescriptorType != UDESC_CS_INTERFACE ||
        header->bDescriptorSubtype != UWH_ACD_HEADER)
    {
        DBG_E(DHOST_AUDIO, ("AUDIO: jaudio_attach - no HEADER descriptor\n"));
        rc = JEINVAL;
        goto Error;
    }

    DBG_V(DHOST_AUDIO, ("AUDIO: ADC v%d.%d (%d streaming interfaces)\n", 
        header->bcdADC[1], header->bcdADC[0], header->bInCollection));

    /* Read Audio Control Interface descriptors and create units */
    rc = audio_parse_ctrl_iface_desc(sc, /* HSU addition */(void*)/* HSU addition */header);
    if (rc)
        goto Error;

    /* Read Audio Streaming Interfaces descriptors and init  */
    for (i = 0; i < header->bInCollection; i++)
    {
        rc = link_streaming_iface_to_unit(sc, header->baInterfaceNr[i]);
        if (rc)
            goto Error;
    }

    /* Init and check units */
    rc = units_init(sc);
    if (rc)
        goto Error;

    sc->cbs = (uwh_audio_callbacks_t *)appmgr_probe(appmgr, sc->sc_dev);
    if (!sc->cbs)
    {
        rc = JENODEV;
        goto Error;
    }

    rc = sc->cbs->attach(sc, &sc->app_ctx);
    if (rc)
        goto Error;

    sc->flags |= AUDIO_APP_ATTACHED;

    /* Open interrupt pipe */
    if (sc->iface->endpoints && sc->cbs->notify)
    {
        usbd_status stat;

        DBG_V(DHOST_AUDIO, ("AUDIO: Open interrupt pipe\n"));

        stat = usbd_open_pipe_intr(sc->iface, 
            sc->iface->endpoints->edesc->bEndpointAddress, 0, &sc->intr.pipe, 
            sc, sc->intr.buf, AUDIO_INTR_BUF_LEN, intr_complete,
            USBD_DEFAULT_INTERVAL);

        rc = usbd_status_to_result(stat);
        if (rc)
        {
            DBG_E(DHOST_AUDIO, ("AUDIO: Open interrupt pipe failed, error - %s"
                "\n", usbd_errstr(stat)));
            goto Error;

        }
    }

    DBG_V(DHOST_AUDIO, ("AUDIO: jaudio_attach success\n"));

    return 0;

Error:
    DBG_E(DHOST_AUDIO, ("AUDIO: jaudio_attach failed, rc=%d\n", rc));
    jaudio_detach(self);
    return rc;
}

/**
 * Function name:  jaudio_detach
 * Description:    Device detach driver method callback.
 *                 This callback is called by the USB Core when a device detach
 *                 is identified.
 *                 The callback must clear all allocated device resources.
 * Parameters:
 *     @self: (IN) Private device data.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t jaudio_detach(jdevice_t self)
{
    jaudio_softc_t *sc = (jaudio_softc_t *)j_device_get_softc(self);
    audio_unit_t **unit;

    DBG_I(DHOST_AUDIO, ("AUDIO: jaudio_detach\n"));

    sc->flags |= AUDIO_DETACH;

    /* Close interrupt pipe */
    if (sc->intr.pipe)
    {
        usbd_abort_pipe(sc->intr.pipe);
        usbd_close_pipe(sc->intr.pipe);
    }
    
    for (unit = &sc->units; *unit; unit = &(*unit)->next)
    {
        if ((*unit)->stream)
            audio_stream_stop((*unit)->stream);
    }

    if (sc->flags & AUDIO_APP_ATTACHED)
        sc->cbs->detach(sc->app_ctx);

    units_uninit(sc);

    return 0;
}

static jresult_t jaudio_suspend(jdevice_t self)
{
    return JEINVAL;
}

static jresult_t jaudio_resume(jdevice_t self)
{
    return JEINVAL;
}

/**
 * Function name:  jaudio_ioctl
 * Description:    IOCTL driver method callback.
 * Parameters:
 *     @self:  (IN) Private device data.
 *     @ioctl: (IN) IOCTL code.
 *     @arg:   (IN) Pointer to IOCTL arguments (NULL if there are no arguments).
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t jaudio_ioctl(jdevice_t self, drv_ioctl_t ioctl, void *arg)
{
    DBG_E(DHOST_AUDIO, ("JAUDIO: Unsupported IOCTL %x\n", ioctl));

    return JEINVAL;
}

/**
 * Function name:  uwh_audio_register
 * Description:    Registers an audio application with the Audio Class Driver.
 *                 The class driver will be operational only after the
 *                 registration function is called and returns successfully.
 * Parameters:
 *     @cbs: (IN) Application callbacks.
 *
 * Return value:   Application handle.
 * Scope:          Global
 **/
uwh_app_h uwh_audio_register(uwh_audio_callbacks_t *cbs)
{
    DBG_I(DHOST_AUDIO, ("AUDIO: uwh_audio_register\n"));

    if (!cbs || !cbs->attach || !cbs->detach)
    {
        DBG_E(DHOST_AUDIO, ("AUDIO: uwh_audio_register - init data is "
            "invalid\n"));
        return NULL;
    }
    return /* HSU addition */(uwh_app_h)/* HSU addition */(appmgr_add(&appmgr, (void *)cbs, cbs->probe));
}

/**
 * Function name:  uwh_audio_unregister
 * Description:    
 * Parameters:
 *     @app_h: Application handle.
 *
 * Return value:   None
 * Scope:          Global
 **/
void uwh_audio_unregister(uwh_app_h app_h)
{
    DBG_I(DHOST_AUDIO, ("AUDIO: uwh_audio_unregister\n"));

    appmgr_remove(&appmgr, app_h);
}

/**
 * Defines the class driver's main initialization function - jaudio_init;
 * initializes the driver's driver_method structure; and registers the driver
 * with the USB Core.
 */
DRIVER_USB_DECLARE(jaudio)

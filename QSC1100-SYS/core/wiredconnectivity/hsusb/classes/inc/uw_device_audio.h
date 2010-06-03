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

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.   http://www/jungo/com */
#ifndef _UW_DEVICE_AUDIO_H_
#define _UW_DEVICE_AUDIO_H_

#include "uw_device_audio_defs.h"

/* ////////////////////////////
 *    Application Functions
 *//////////////////////////// */

/**
 * Function name:  uwd_audio_app_init
 * Description:    Application initialization function.
 * Parameters:
 *     @fd_ctx:  (IN)  Audio function driver context.
 *     @app_ctx: (OUT) Pointer to application context.
 *     @config:  (OUT) Pointer to a pointer to an audio device configuration
 *                     structure. The application should create the structure
 *                     and fill it with the required configuration information,
 *                     including callback functions for use by the FD.
 *                     NOTE: The configuration data should remain available
 *                     and unchanged until the application is uninitialized
 *                     (i.e., until the (*config)->uninit callback is called).
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Global
 **/
jresult_t uwd_audio_app_init(uwd_audio_drv_h fd_ctx, uwd_audio_app_h *app_ctx,
    uwd_audio_config_t **config);


/* ////////////////////////////
 *      Audio FD Functions
 *//////////////////////////// */
/* The Audio FD functions are described in jslave/fd/audio/fd_audio.c */

/* Audio Stream Manipulation Functions */

jresult_t uwd_audio_stream_write(uwd_audio_drv_h fd_ctx, 
    uwd_audio_stream_h stream, void *buf, juint32_t len,
    uwd_audio_stream_cb_t cb, void *arg);

jresult_t uwd_audio_stream_read(uwd_audio_drv_h fd_ctx, 
    uwd_audio_stream_h stream, void *buf, juint32_t len,
    uwd_audio_stream_cb_t cb, void *arg);

void uwd_audio_stream_abort(uwd_audio_drv_h fd_ctx, uwd_audio_stream_h stream);

jresult_t uwd_audio_stream_sync(uwd_audio_drv_h fd_ctx, 
    uwd_audio_stream_h stream, jint32_t audio_frames);


/** Status Interrupt Endpoint Functions
 *  Applicable only if the device is implemented to include a status interrupt
 *  endpoint (optional).
 **/

jresult_t uwd_audio_entity_intr(uwd_audio_drv_h fd_ctx, 
    uwd_audio_entity_h entity);

jresult_t uwd_audio_memory_intr(uwd_audio_drv_h fd_ctx, 
    uwd_audio_entity_h entity);

jresult_t uwd_audio_stream_intr(uwd_audio_drv_h fd_ctx, 
    uwd_audio_stream_h stream);

jresult_t uwd_audio_endpoint_intr(uwd_audio_drv_h fd_ctx, 
    uwd_audio_stream_h stream);

#endif


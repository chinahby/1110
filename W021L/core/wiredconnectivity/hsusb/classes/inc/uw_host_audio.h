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

#ifndef __UW_HOST_AUDIO_H__
#define __UW_HOST_AUDIO_H__

#include <jos.h>
#include <jusb.h>
#include "uw_host_appmgr.h"

enum uwh_audio_ctrl_desc_subtype {
    UWH_ACD_UNDEFINED            = 0x00,
    UWH_ACD_HEADER               = 0x01,
    UWH_ACD_INPUT_TERMINAL       = 0x02,
    UWH_ACD_OUTPUT_TERMINAL      = 0x03,
    UWH_ACD_MIXER_UNIT           = 0x04,
    UWH_ACD_SELECTOR_UNIT        = 0x05,
    UWH_ACD_FEATURE_UNIT         = 0x06,
    UWH_ACD_PROCESSING_UNIT      = 0x07,
    UWH_ACD_EXTENSION_UNIT       = 0x08
};

/* Audio Class-Specific AS Interface Descriptor Subtypes */
enum uwh_audio_stream_desc_subtype {
    UWH_ASD_UNDEFINED            = 0x00,
    UWH_ASD_GENERAL              = 0x01,
    UWH_ASD_FORMAT_TYPE          = 0x02,
    UWH_ASD_FORMAT_SPECIFIC      = 0x03
};

/* Audio Interface Subclass Codes */
enum uwh_audio_iface_subclass_code {
    UWH_AI_UNDEFINED             = 0x00,
    UWH_AI_AUDIOCONTROL          = 0x01,
    UWH_AI_AUDIOSTREAMING        = 0x02,
    UWH_AI_MIDISTREAMING         = 0x03
};

/* Audio Class-Specific Request Codes */
typedef enum uwh_audio_req_code {
    UWH_ARC_UNDEFINED             = 0x00,
    UWH_ARC_SET_CUR               = 0x01,
    UWH_ARC_GET_CUR               = 0x81,
    UWH_ARC_SET_MIN               = 0x02,
    UWH_ARC_GET_MIN               = 0x82,
    UWH_ARC_SET_MAX               = 0x03,
    UWH_ARC_GET_MAX               = 0x83,
    UWH_ARC_SET_RES               = 0x04,
    UWH_ARC_GET_RES               = 0x84,
    UWH_ARC_SET_MEM               = 0x05,
    UWH_ARC_GET_MEM               = 0x85,
    UWH_ARC_GET_STAT              = 0xFF
} uwh_audio_req_code_t;

/* Terminal Control Selectors */
enum uwh_audio_terminal_cs {
    UWH_AT_CONTROL_UNDEFINED     = 0x00,
    UWH_AT_COPY_PROTECT_CONTROL  = 0x01
};

/* Endpoint Control Selectors */
enum uwh_audio_endpoint_cs {
    UWH_AEP_CONTROL_UNDEFINED             = 0x00,
    UWH_AEP_SAMPLING_FREQ_CONTROL         = 0x01,
    UWH_AEP_PITCH_CONTROL                 = 0x02
};

/* Feature Unit Control Selectors */
enum uwh_audio_feature_unit_cs {
    UWH_AFU_CONTROL_UNDEFINED             = 0x00,
    UWH_AFU_MUTE_CONTROL                  = 0x01,
    UWH_AFU_VOLUME_CONTROL                = 0x02,
    UWH_AFU_BASS_CONTROL                  = 0x03,
    UWH_AFU_MID_CONTROL                   = 0x04,
    UWH_AFU_TREBLE_CONTROL                = 0x05,
    UWH_AFU_GRAPHIC_EQUALIZER_CONTROL     = 0x06,
    UWH_AFU_AUTOMATIC_GAIN_CONTROL        = 0x07,
    UWH_AFU_DELAY_CONTROL                 = 0x08,
    UWH_AFU_BASS_BOOST_CONTROL            = 0x09,
    UWH_AFU_LOUDNESS_CONTROL              = 0x0A
};

/* Format Type Codes */
enum uwh_audio_format_type {
    UWH_AF_TYPE_UNDEFINED         = 0x00,
    UWH_AF_TYPE_I                 = 0x01,
    UWH_AF_TYPE_II                = 0x02,
    UWH_AF_TYPE_III               = 0x03
};

/* Audio Data Format Type I,II,III Codes */
enum uwh_audio_data_format_type {
    UWH_ADF_I_UNDEFINED                   = 0x0000,
    UWH_ADF_I_PCM                         = 0x0001,
    UWH_ADF_I_PCM8                        = 0x0002,
    UWH_ADF_I_IEEE_FLOAT                  = 0x0003,
    UWH_ADF_I_ALAW                        = 0x0004,
    UWH_ADF_I_MULAW                       = 0x0005,
    UWH_ADF_II_UNDEFINED                  = 0x1000,
    UWH_ADF_II_MPEG                       = 0x1001,
    UWH_ADF_II_AC_3                       = 0x1002,
    UWH_ADF_III_UNDEFINED                 = 0x2000,
    UWH_ADF_III_IEC1937_AC_3              = 0x2001,
    UWH_ADF_III_IEC1937_MPEG_1_L1         = 0x2002,
    UWH_ADF_III_IEC1937_MPEG_1_L2_L3      = 0x2003,
    UWH_ADF_III_IEC1937_MPEG_2_NOEXT      = 0x2003,
    UWH_ADF_III_IEC1937_MPEG_2_EXT        = 0x2004,
    UWH_ADF_III_IEC1937_MPEG_2_L1         = 0x2005,
    UWH_ADF_III_IEC1937_MPEG_2_L2_L3      = 0x2006
};

/* USB Terminal Types (USB, Input and Output)*/
enum uwh_audio_terminal_type {
    
/* USB Terminal Types */
    UWH_ATT_USB_UNDEFINED                 = 0x0100,
    UWH_ATT_USB_STREAMING                 = 0x0101,
    UWH_ATT_USB_VENDOR_SPECIFIC           = 0x01FF,

/* Input Terminal Types */
    UWH_ATT_INPUT_UNDEFINED               = 0X0200,
    UWH_ATT_MICROPHONE                    = 0X0201,
    UWH_ATT_DESKTOP_MICROPHONE            = 0X0202,
    UWH_ATT_PERSONAL_MICROPHONE           = 0X0203,
    UWH_ATT_OMNI_DIRECTIONAL_MICROPHONE   = 0X0204,
    UWH_ATT_MICROPHONE_ARRAY              = 0X0205,
    UWH_ATT_PROCESSING_MICROPHONE_ARRAY   = 0X0206,

/* Output Terminal Types */
    UWH_ATT_OUTPUT_UNDEFINED              = 0X0300,
    UWH_ATT_SPEAKER                       = 0X0301,
    UWH_ATT_HEADPHONES                    = 0X0302,
    UWH_ATT_HEAD_MOUNTED_DISPLAY_AUDIO    = 0X0303,
    UWH_ATT_DESKTOP_SPEAKER               = 0X0304,
    UWH_ATT_ROOM_SPEAKER                  = 0X0305,
    UWH_ATT_COMMUNICATION_SPEAKER         = 0X0306,
    UWH_ATT_LOW_FREQUENCY_EFFECTS_SPEAKER = 0X0307
};

/* ADC v1.0 Descriptors */

/* Class Specific Audio Control Interface Descriptors */

typedef __PACKED_PRE__ struct {
    uByte    bLength;
    uByte    bDescriptorType;
    uByte    bDescriptorSubtype;
    uByte    bTerminalID;
    uWord    wTerminalType; /* enum uwh_audio_terminal_type */
    uByte    bAssocTerminal;
    uByte    bNrChannels;
    uWord    wChannelConfig;
    uByte    iChannelNames;
    uByte    iTerminal;
} __PACKED_POST__ uwh_audio_input_terminal_desc_t;

typedef __PACKED_PRE__ struct {
    uByte    bLength;
    uByte    bDescriptorType;
    uByte    bDescriptorSubtype;
    uByte    bTerminalID;
    uWord    wTerminalType; /* enum uwh_audio_terminal_type */
    uByte    bAssocTerminal;
    uByte    bSourceID;
    uByte    iTerminal;
}  __PACKED_POST__ uwh_audio_output_terminal_desc_t;

/* Class Specific Audio Streaming Interface Descriptors */
typedef __PACKED_PRE__ struct {
    uByte    bLength;
    uByte    bDescriptorType;    
    uByte    bDescriptorSubtype; /* ASD_GENERAL */
    uByte    bTerminalLink;
    uByte    bDelay;
    uWord    wFormatTag; /* enum uwh_audio_data_format_type */
} __PACKED_POST__ uwh_audio_sif_desc_t;

typedef juint8_t uwh_audio_samp_freq_t[3];

/* Audio Type I Format Type Descriptor */
typedef __PACKED_PRE__ struct {
    uByte    bLength;
    uByte    bDescriptorType;
    uByte    bDescriptorSubtype;
    uByte    bFormatType; /* enum uwh_audio_format_type */
    uByte    bNrChannels;
    uByte    bSubframeSize;
    uByte    bBitResolution;
    uByte    bSamFreqType;
    uwh_audio_samp_freq_t tSamFreq[1];
} __PACKED_POST__ uwh_audio_format_I_desc_t;

/* Audio Type II Format Type Descriptor */
typedef __PACKED_PRE__ struct {
    uByte    bLength;
    uByte    bDescriptorType;
    uByte    bDescriptorSubtype;
    uByte    bFormatType; /* enum uwh_audio_format_type */
    uWord    wMaxBitRate;
    uWord    wSamplesPerFrame;
    uByte    bSamFreqType;
    uwh_audio_samp_freq_t tSamFreq[1];
} __PACKED_POST__ uwh_audio_format_II_desc_t;

/* Audio Type III Format Type Descriptor */
typedef uwh_audio_format_I_desc_t uwh_audio_format_III_desc_t;

typedef __PACKED_PRE__ struct {
    uByte    bLength;
    uByte    bDescriptorType;
    uByte    bDescriptorSubtype;
    uByte    bmAttributes;
    uByte    bLockDelayUnits;
    uWord    wLockDelay;
} __PACKED_POST__ uwh_audio_isoc_ep_desc_t;

/* Feature unit descriptor */
typedef __PACKED_PRE__ struct {
    uByte    bLength;
    uByte    bDescriptorType;
    uByte    bDescriptorSubtype;
    uByte    bUnitID;
    uByte    bSourceID;
    uByte    bControlSize;
} __PACKED_POST__ uwh_audio_feature_unit_desc_t;

/* Audio Class Driver Context */
typedef void *uwh_audio_drv_h;

/* Audio Application Context */
typedef void *uwh_audio_app_h;

/* Audio Entity Type 
 * (Copy of Class Specific Audio Control Interface Descriptor) */
typedef enum uwh_audio_ctrl_desc_subtype uwh_audio_entity_type_t;

typedef struct
{
    uwh_audio_sif_desc_t            *sif_desc;
    usb_endpoint_descriptor_t       *ep_desc;
    uwh_audio_isoc_ep_desc_t        *ep_audio_desc;

    enum uwh_audio_format_type      format_type;
    union {
        uwh_audio_format_I_desc_t   *type_I;
        uwh_audio_format_II_desc_t  *type_II;
        uwh_audio_format_III_desc_t *type_III;
    } format;
} uwh_audio_stream_iface_t;

/* Audio Terminals and Units 
 * (Only input and output terminals currently supported) */
typedef struct {
    uwh_audio_input_terminal_desc_t *terminal_desc;
    juint8_t sif_num;
    uwh_audio_stream_iface_t *sif;
} uwh_audio_input_terminal_t;

typedef struct {
    uwh_audio_output_terminal_desc_t *terminal_desc;
    juint8_t sif_num;
    uwh_audio_stream_iface_t *sif;
} uwh_audio_output_terminal_t;

/* Audio entity */
typedef struct {
    juint8_t entity_id;
    uwh_audio_entity_type_t type;

    union {
        uwh_audio_input_terminal_t  *in_trm;
        uwh_audio_output_terminal_t *out_trm;
        void                        *unit_desc;
    } u;
} uwh_audio_entity_t;

/* Audio Notification Information */
typedef enum {
    UWH_AN_CTRL_INTERFACE = 0,
    UWH_AN_STREAM_INTERFACE = 1,
    UWH_AN_ENDPOINT = 2
} uwh_audio_notify_originator_t;

typedef struct {
    juint_t  memory_contents_changed : 1;
    juint_t  interrupt_pending       : 1;

    uwh_audio_notify_originator_t originator_type;
    union {
        juint8_t entity_id;
        juint8_t stream_iface;
        juint8_t ep;
    } originator;
} uwh_audio_notify_info_t;

/* ///////////////////////////////
     Audio Application Callbacks
 * ////////////////////////////// */

typedef jresult_t (*uwh_audio_attach_cb_t)(uwh_audio_drv_h drv_ctx, 
    uwh_audio_app_h *app_ctx);

typedef void (*uwh_audio_detach_cb_t)(uwh_audio_app_h app_ctx);

typedef void (*uwh_audio_notify_cb_t)(uwh_audio_app_h app_ctx,
    uwh_audio_notify_info_t *notify_info);

typedef struct {
    uwh_app_probe_cb_t    probe;
    uwh_audio_attach_cb_t attach;
    uwh_audio_detach_cb_t detach;
    uwh_audio_notify_cb_t notify;
} uwh_audio_callbacks_t;

/* ///////////////////////////////
    Audio Class Driver Functions
 * ////////////////////////////// */

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
uwh_app_h uwh_audio_register(uwh_audio_callbacks_t *cbs);

/**
* Function name:  uwh_audio_unregister
* Description:    Unregisters an audio application from the Audio Class Driver.
* Parameters:
*     @app_h: Application handle.
*
* Return value:   None
* Scope:          Global
**/
void uwh_audio_unregister(uwh_app_h app_h);

/* /////////////////////////////////////////////
   Audio Units and Terminals Access Functions 
 * ///////////////////////////////////////////// */

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
uwh_audio_entity_t *uwh_audio_get_first_entity(uwh_audio_drv_h drv_ctx);

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
    uwh_audio_entity_t *cur_entity);

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
    juint8_t entity_id);

/* ////////////////
   Audio Requests 
 * /////////////// */

typedef enum {
    UWH_AR_ENDPOINT,
    UWH_AR_CTRL_INTERFACE,
    UWH_AR_STREAM_INTERFACE
} uwh_audio_ctrl_req_type_t;

typedef struct {
    uwh_audio_entity_t        *entity;

    void                      *buffer;
    juint32_t                  buf_len;

    uwh_audio_ctrl_req_type_t type;
    uwh_audio_req_code_t      code;
    juint8_t                   ctrl_sel; /* enum xxx_cs */
    juint16_t                  value;    /* can be used for channel */
} uwh_audio_ctrl_req_t;

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
jresult_t uwh_audio_ctrl_req(uwh_audio_drv_h drv_ctx, uwh_audio_ctrl_req_t *req);

/* ////////////////
   Audio Streaming 
 * /////////////// */

/**
* Function name:  uwh_audio_set_stream
* Description:    Sets the alternate setting of the AudioStreaming inteface
*                 for the given USB streaming terminal.
* Parameters:
*     @drv_ctx: (IN) Audio driver context 
*     @trm:     (IN) Pointer to a USB streaming terminal entity structure.
*     @sif:     (IN) Pointer to an AudioStreaming interface (ASI) structure.
*
* Return value:   0 on success, otherwise an error code.
* Scope:          Global
**/
jresult_t uwh_audio_set_stream(uwh_audio_drv_h drv_ctx, uwh_audio_entity_t *trm, 
    uwh_audio_stream_iface_t *sif);

/**
 * Function name:  uwh_audio_samp_freq_req
 * Description:    Sends SAMPLING_FREQ_CONTROL request to given terminal's
 *                 endpoint.
 * Parameters:
 *     @drv_ctx: (IN) Audio driver context 
 *     @trm:     (IN) Pointer to a USB streaming terminal entity structure.
 *     @code:    (IN) Request code.
 *     @freq_hz: (IN/OUT) Pointer to sampling frequency in Hz.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Global
 **/
jresult_t uwh_audio_samp_freq_req(uwh_audio_drv_h drv_ctx, 
    uwh_audio_entity_t *trm, uwh_audio_req_code_t code, juint32_t *freq_hz);

/**
 * Function type:  uwh_audio_write_cb_t
 * Description:    Write completion application callback.
 * Parameters:
 *     @arg:    (IN) Pointer to callback arguments.
 *     @status: (IN) Write transfer status.
 *
 * Return value:   None
 **/
typedef void (*uwh_audio_write_cb_t)(void *arg, jresult_t status);

/**
* Function name:  uwh_audio_write_data
* Description:    Writes data to the device using the given USB audio stream.
* Parameters:
*     @entity:  (IN) Pointer to a USB streaming terminal entity structure.
*     @buf:     (IN) Handle to a memory buffer containing the data to write.
*     @len:     (IN) Data length, in bytes.
*     @cb:      (IN) Write completion application callback:
*     @arg:     (IN) Pointer to arguments for the application's write
*                    completion callback (cb).
*
* Return value:   0 on success, otherwise an error code.
* Scope:          Global
**/
jresult_t uwh_audio_write_data(uwh_audio_entity_t *entity, void *buf, 
    juint32_t len, uwh_audio_write_cb_t cb, void *arg);

/**
 * Function type:  uwh_audio_read_cb_t
 * Description:    Application read callback.
 * Parameters:
 *     @arg:    (IN) Pointer to callback arguments.
 *     @status: (IN) Read stream status.
 *
 * Return value:   None
 **/
typedef void (*uwh_audio_read_cb_t)(void *arg, jresult_t status);

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
*     @arg:         (IN) Pointer to arguments for the application's read
*                        callback (cb).
*
* Return value:   0 on success, otherwise an error code.
* Scope:          Global
**/
jresult_t uwh_audio_read_start(uwh_audio_entity_t *entity, juint16_t buf_time_ms, 
    uwh_audio_read_cb_t cb, void *arg);

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
juint32_t uwh_audio_read_get_data_size(uwh_audio_entity_t *entity);

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
juint32_t uwh_audio_read_get_frame_size(uwh_audio_entity_t *entity);

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
    void *buffer, juint32_t size);

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
void uwh_audio_stream_abort(uwh_audio_drv_h drv_ctx, 
    uwh_audio_entity_t *entity);

/* ////////////////////
   Utility Functions 
 * /////////////////// */

/**
* Function name:  uwh_audio_samp_freq_to_uint32
* Description:    Converts sampling frequency from a 3-bytes format to Hz.
* Parameters:
*     @freq: (IN) Sampling frequency expressed in a 3-bytes format.
*
* Return value:   Frequency in units of Hz.
* Scope:          Global
**/
juint32_t uwh_audio_samp_freq_to_uint32(uwh_audio_samp_freq_t *freq);

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
    uwh_audio_samp_freq_t *freq);

#endif

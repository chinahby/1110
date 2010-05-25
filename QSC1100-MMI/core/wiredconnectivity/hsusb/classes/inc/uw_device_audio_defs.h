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
#ifndef _UW_DEVICE_AUDIO_DEFS_H_
#define _UW_DEVICE_AUDIO_DEFS_H_

#include <jtypes.h>

/* Audio FD Context Type */
typedef void *uwd_audio_drv_h;

/* Application Context Type */
typedef void *uwd_audio_app_h;

/* Audio Entity Types */
typedef enum {
    UWD_AUDIO_ET_INPUT_TERMINAL  = 0x02,
    UWD_AUDIO_ET_OUTPUT_TERMINAL = 0x03,
    UWD_AUDIO_ET_MIXER_UNIT      = 0x04,
    UWD_AUDIO_ET_SELECTOR_UNIT   = 0x05,
    UWD_AUDIO_ET_FEATURE_UNIT    = 0x06,
    UWD_AUDIO_ET_PROCESSING_UNIT = 0x07,
    UWD_AUDIO_ET_EXTENSION_UNIT  = 0x08
} uwd_audio_entity_type_t;

/* Audio Terminal Types */
typedef enum {
/* USB Terminal Types */
    UWD_AUDIO_TT_USB_UNDEFINED                 = 0x0100,
    UWD_AUDIO_TT_USB_STREAMING                 = 0x0101,
    UWD_AUDIO_TT_USB_VENDOR_SPECIFIC           = 0x01FF,

/* Input Terminal Types */
    UWD_AUDIO_TT_INPUT_UNDEFINED               = 0x0200,
    UWD_AUDIO_TT_MICROPHONE                    = 0x0201,
    UWD_AUDIO_TT_DESKTOP_MICROPHONE            = 0x0202,
    UWD_AUDIO_TT_PERSONAL_MICROPHONE           = 0x0203,
    UWD_AUDIO_TT_OMNI_DIRECTIONAL_MICROPHONE   = 0x0204,
    UWD_AUDIO_TT_MICROPHONE_ARRAY              = 0x0205,
    UWD_AUDIO_TT_PROCESSING_MICROPHONE_ARRAY   = 0x0206,

/* Output Terminal Types */
    UWD_AUDIO_TT_OUTPUT_UNDEFINED              = 0x0300,
    UWD_AUDIO_TT_SPEAKER                       = 0x0301,
    UWD_AUDIO_TT_HEADPHONES                    = 0x0302,
    UWD_AUDIO_TT_HEAD_MOUNTED_DISPLAY_AUDIO    = 0x0303,
    UWD_AUDIO_TT_DESKTOP_SPEAKER               = 0x0304,
    UWD_AUDIO_TT_ROOM_SPEAKER                  = 0x0305,
    UWD_AUDIO_TT_COMMUNICATION_SPEAKER         = 0x0306,
    UWD_AUDIO_TT_LOW_FREQUENCY_EFFECTS_SPEAKER = 0x0307
} uwd_audio_trm_type_t;

/* Audio Class-Specific Request Codes */
typedef enum {
    UWD_AUDIO_RC_SET_CUR  = 0x01,
    UWD_AUDIO_RC_GET_CUR  = 0x81,
    UWD_AUDIO_RC_SET_MIN  = 0x02,
    UWD_AUDIO_RC_GET_MIN  = 0x82,
    UWD_AUDIO_RC_SET_MAX  = 0x03,
    UWD_AUDIO_RC_GET_MAX  = 0x83,
    UWD_AUDIO_RC_SET_RES  = 0x04,
    UWD_AUDIO_RC_GET_RES  = 0x84,
    UWD_AUDIO_RC_SET_MEM  = 0x05,
    UWD_AUDIO_RC_GET_MEM  = 0x85,
    UWD_AUDIO_RC_GET_STAT = 0xFF
} uwd_audio_req_code_t;

/* Endpoint Control Selectors */
typedef enum {
    UWD_AUDIO_EP_SAMPLING_FREQ_CONTROL = 0x01,
    UWD_AUDIO_EP_PITCH_CONTROL         = 0x02
} uwd_audio_endpoint_cs_t;

/* Terminal Control Selectors */
typedef enum {
    UWD_AUDIO_TR_CONTROL_UNDEFINED     = 0x00,
    UWD_AUDIO_TR_COPY_PROTECT_CONTROL  = 0x01
} uwd_audio_terminal_cs_t;

/* Feature Unit Control Selectors */
typedef enum {
    UWD_AUDIO_FU_CONTROL_UNDEFINED         = 0x00,
    UWD_AUDIO_FU_MUTE_CONTROL              = 0x01,
    UWD_AUDIO_FU_VOLUME_CONTROL            = 0x02,
    UWD_AUDIO_FU_BASS_CONTROL              = 0x03,
    UWD_AUDIO_FU_MID_CONTROL               = 0x04,
    UWD_AUDIO_FU_TREBLE_CONTROL            = 0x05,
    UWD_AUDIO_FU_GRAPHIC_EQUALIZER_CONTROL = 0x06,
    UWD_AUDIO_FU_AUTOMATIC_GAIN_CONTROL    = 0x07,
    UWD_AUDIO_FU_DELAY_CONTROL             = 0x08,
    UWD_AUDIO_FU_BASS_BOOST_CONTROL        = 0x09,
    UWD_AUDIO_FU_LOUDNESS_CONTROL          = 0x0A
} uwd_audio_feature_unit_cs_t;

/* Extension Unit Control Selectors */
typedef enum {
    UWD_AUDIO_EU_CONTROL_UNDEFINED = 0x00,
    UWD_AUDIO_EU_ENABLE_CONTROL    = 0x01
} uwd_audio_extension_unit_cs_t;

/* Special Audio Channel Selection Flags */
#define UWD_AUDIO_CTRL_MASTER_CH  0x00 /* All channels */
#define UWD_AUDIO_CTRL_ALL_CH     0xff /* The master channel (channel 0) */

/* Audio Channel Config */
#define UWD_AUDIO_CC_LEFT_FRONT            (1 << 0)
#define UWD_AUDIO_CC_RIGHT_FRONT           (1 << 1)
#define UWD_AUDIO_CC_CENTER_FRONT          (1 << 2)
#define UWD_AUDIO_CC_LOW_FREQ_ENHANCEMENT  (1 << 3)
#define UWD_AUDIO_CC_LEFT_SURROUND         (1 << 4)
#define UWD_AUDIO_CC_RIGHT_SURROUND        (1 << 5)
#define UWD_AUDIO_CC_LEFT_OF_CENTER        (1 << 6)
#define UWD_AUDIO_CC_RIGHT_OF_CENTER       (1 << 7)
#define UWD_AUDIO_CC_SURROUND              (1 << 8)
#define UWD_AUDIO_CC_SIDE_LEFT             (1 << 9)
#define UWD_AUDIO_CC_SIDE_RIGHT            (1 << 10)
#define UWD_AUDIO_CC_TOP                   (1 << 11)

/* Feature Unit Controls */
#define UWD_AUDIO_FC_MUTE              (1 << 0)
#define UWD_AUDIO_FC_VOLUME            (1 << 1)
#define UWD_AUDIO_FC_BASS              (1 << 2)
#define UWD_AUDIO_FC_MID               (1 << 3)
#define UWD_AUDIO_FC_TREBLE            (1 << 4)
#define UWD_AUDIO_FC_GRAPHIC_EQUALIZER (1 << 5)
#define UWD_AUDIO_FC_AUTOMATIC_GAIN    (1 << 6)
#define UWD_AUDIO_FC_DELAY             (1 << 7)
#define UWD_AUDIO_FC_BASS_BOOST        (1 << 8)
#define UWD_AUDIO_FC_LOUDNESS          (1 << 9)

/* Extension Unit Controls */
#define UWD_AUDIO_EC_ENABLE_PROCESSING (1 << 0)

/* Audio Entity Handle */
typedef struct uwd_audio_entity_s *uwd_audio_entity_h;

/**
 * Function type:  uwd_audio_entity_req_cb_t
 * Description:    Application AudioControl interface request handler for a
 *                 specific audio entity.
 * Requirement:    Optional
 * Parameters:
 *     @app_ctx: (IN)  Application context.
 *     @entity:  (IN)  Pointer to an entity structure.
 *     @code:    (IN)  Audio request code - see uwd_audio_entity_req_cb_t.
 *     @value:   (IN)  wValue Set/Get Request field value - see explanation in
 *                     the Audio Class Specification.
 *                     For requests that receive the channel number (CN) in the
 *                     low byte (e.g., the Feature Unit control requests), use
 *                     the UWD_AUDIO_CTRL_XXX flags to indicate special audio
 *                     channel targets: UWD_AUDIO_CTRL_ALL_CH - all channels;
 *                     UWD_AUDIO_CTRL_MASTER_CH - the master channel (CN 0).
 *     @len:     (IN)  Length of the @data buffer, in bytes.
 *     @data: (IN/OUT) Pointer to a buffer containing the parameters for the
 *                     request. Input for a Set request; output for a Get
 *                     request.
 *
 * Return value:   0 on success, otherwise an error code.
 **/
typedef jresult_t (*uwd_audio_entity_req_cb_t)(uwd_audio_app_h app_ctx, 
    uwd_audio_entity_h entity, uwd_audio_req_code_t code, juint16_t value, 
    juint16_t len, void *data);

/* Audio Entity */
typedef struct uwd_audio_entity_s {
    juint8_t                   id;
    uwd_audio_entity_type_t   type;
    void                      *data;
    char                      *string;
    uwd_audio_entity_req_cb_t req_cb;   /* Optional */
    void                      *app_priv;
} uwd_audio_entity_t;

/* Input Terminal */
typedef struct {
    uwd_audio_trm_type_t trm_type;
    juint8_t              assoc_terminal;
    juint8_t              nr_channels;
    juint16_t             channel_config;
} uwd_audio_in_trm_t;

/* Output Terminal */
typedef struct {
    uwd_audio_trm_type_t trm_type;
    juint8_t              assoc_terminal;
    juint8_t              source_id;
} uwd_audio_out_trm_t;

/* Feature Unit */
typedef struct {
    juint8_t              source_id;
    juint8_t              nr_channels;
    juint16_t             *controls;
} uwd_audio_feature_unit_t;

/* Selector Unit */
typedef struct {
    juint8_t                  nr_in_pins;
    juint8_t                  *source_id;
} uwd_audio_selector_unit_t;

/* Extension Unit */
typedef struct {
    juint16_t                 extension_code;
    juint8_t                  nr_in_pins;
    juint8_t                  *source_id;
    juint8_t                  nr_channels;
    juint16_t                 channel_config;
    juint8_t                  controls;     
} uwd_audio_extension_unit_t;

/* Audio Data Format Type I,II,III Codes */
typedef enum {
    UWD_AUDIO_DF_I_UNDEFINED                   = 0x0000,
    UWD_AUDIO_DF_I_PCM                         = 0x0001,
    UWD_AUDIO_DF_I_PCM8                        = 0x0002,
    UWD_AUDIO_DF_I_IEEE_FLOAT                  = 0x0003,
    UWD_AUDIO_DF_I_ALAW                        = 0x0004,
    UWD_AUDIO_DF_I_MULAW                       = 0x0005,
    UWD_AUDIO_DF_II_UNDEFINED                  = 0x1000,
    UWD_AUDIO_DF_II_MPEG                       = 0x1001,
    UWD_AUDIO_DF_II_AC_3                       = 0x1002,
    UWD_AUDIO_DF_III_UNDEFINED                 = 0x2000,
    UWD_AUDIO_DF_III_IEC1937_AC_3              = 0x2001,
    UWD_AUDIO_DF_III_IEC1937_MPEG_1_L1         = 0x2002,
    UWD_AUDIO_DF_III_IEC1937_MPEG_1_L2_L3      = 0x2003,
    UWD_AUDIO_DF_III_IEC1937_MPEG_2_NOEXT      = 0x2003,
    UWD_AUDIO_DF_III_IEC1937_MPEG_2_EXT        = 0x2004,
    UWD_AUDIO_DF_III_IEC1937_MPEG_2_L1         = 0x2005,
    UWD_AUDIO_DF_III_IEC1937_MPEG_2_L2_L3      = 0x2006
} uwd_audio_format_type_t;

/* AudioStreaming Interface Handle */
typedef struct uwd_audio_stream_s *uwd_audio_stream_h;
/* AudioStreaming Interface Alternate Setting Handle */
typedef struct uwd_audio_stream_alt_s *uwd_audio_stream_alt_h;

/**
 * Function type:  uwd_audio_stream_select_cb_t
 * Description:    Notifies the application of a need to change the audio
 *                 settings of an AudioStreaming interface - either via a USB
 *                 SET_INTERFACE request, or via an audio sampling frequency
 *                 control SAMPLING_FREQ_CONTROL Set request.
 * Requirement:    Mandatory
 * Parameters:
 *     @app_ctx:   (IN) Application context.
 *     @stream:    (IN) Pointer to an AudioStreaming interface structure.
 *     @stream_alt (IN) Pointer to an AudioStreaming  interface alternate
 *                      setting structure.
 *                      NULL represents alternate setting 0, which indicates
 *                      that the host requested to stop the audio stream.
 *     @samp_freq  (IN) Sampling frequency, in Hz.
 *
 * Return value:   None
 **/
typedef void (*uwd_audio_stream_select_cb_t)(uwd_audio_app_h app_ctx,
    uwd_audio_stream_h stream, uwd_audio_stream_alt_h stream_alt, 
    juint32_t samp_freq);

/**
 * Function type:  uwd_audio_stream_req_cb_t
 * Description:    Application AudioStreaming interface control request handler.
 * Requirement:    Optional
 * Parameters:
 *     @app_ctx:   (IN) Application context.
 *     @stream:    (IN) Pointer to an AudioStreaming interface structure.
 *     @stream_alt (IN) Pointer to an AudioStreaming  interface alternate
 *                      setting structure.
 *                      NULL represents alternate setting 0, which indicates
 *                      that the host requested to stop the audio stream.
 *     @samp_freq  (IN) Sampling frequency, in Hz.
 *
 * Return value:   None
 **/
typedef jresult_t (*uwd_audio_stream_req_cb_t)(uwd_audio_app_h app_ctx, 
    uwd_audio_stream_h stream, uwd_audio_req_code_t code, juint16_t value, 
    juint16_t len, void *data);

/**
 * Function type:  uwd_audio_ep_req_cb_t
 * Description:    Application AudioStreaming endpoint control request handler.
 * Requirement:    Optional
 * Parameters:
 *     @app_ctx: (IN)  Application context.
 *     @stream:  (IN)  Pointer to an AudioStreaming interface structure.
 *     @code:    (IN)  Audio request code - see uwd_audio_entity_req_cb_t.
 *     @cs:      (IN)  Endpoint control selector code - see
 *                     uwd_audio_endpoint_cs_t.
 *     @len:     (IN)  Length of the @data buffer, in bytes.
 *     @data: (IN/OUT) Pointer to a buffer containing the parameters for the
 *                     request. Input for a Set request; output for a Get
 *                     request.
 *
 * Return value:   None
 **/
typedef jresult_t (*uwd_audio_ep_req_cb_t)(uwd_audio_app_h app_ctx, 
    uwd_audio_stream_h stream, uwd_audio_req_code_t code, 
    uwd_audio_endpoint_cs_t cs, juint16_t len, void *data);

/* For Audio Streaming Endpoints only interval 1 ms can be used */
/* (according to Audio Device 1.0 specification)                */
typedef enum {
    UWD_AUDIO_NO_DATA = 0,
    UWD_AUDIO_IVAL_1_MS,
    UWD_AUDIO_IVAL_2_MS,
    UWD_AUDIO_IVAL_4_MS,
    UWD_AUDIO_IVAL_8_MS,
    UWD_AUDIO_IVAL_16_MS,
    UWD_AUDIO_IVAL_32_MS,
    UWD_AUDIO_IVAL_64_MS,
    UWD_AUDIO_IVAL_128_MS
} uwd_audio_ival_t;

/* Streaming Interface Alternate Setting */
typedef struct uwd_audio_stream_alt_s {
    uwd_audio_ival_t         ep_ival;
   /* The Audio FD will calculate the maximum endpoint packet size */
#define UWD_AUDIO_FD_CALC_MAX_PACKET 0
/* The Audio FD will calculate the maximum endpoint packet size, allowing for
 * clocks synchronization using uwd_audio_stream_sync() */
#define UWD_AUDIO_FD_CALC_MAX_PACKET_SYNC 0xffff
    juint16_t                 ep_max_packet;
    juint8_t                  trm_link;
    juint8_t                  delay;
    uwd_audio_format_type_t  format_type;
    void                     *format;
} uwd_audio_stream_alt_t;

/* AudioStreaming Interface (ASI) Structure */
typedef struct uwd_audio_stream_s {
    juint8_t                      alts_num;
    uwd_audio_stream_alt_t       *alts;

    uwd_audio_stream_select_cb_t select_cb;        /* Mandatory */
    uwd_audio_stream_req_cb_t    req_cb;        /* Optional */
    uwd_audio_ep_req_cb_t        ep_req_cb;     /* Optional */
    
    void                         *app_priv;
    jbool_t                       read;          /* Initialized by the FD */
} uwd_audio_stream_t;

typedef struct {
    juint32_t min;
    juint32_t max;
} uwd_audio_freq_continuous_t;

typedef struct {
    juint8_t freqs_num;
    juint32_t *freqs;
} uwd_audio_freq_discrete_t;

/* Audio Type I Format Type */
typedef struct {
    juint8_t                     nr_channels;
    juint8_t                     bit_resolution;
    uwd_audio_freq_discrete_t   *freq_discrete;
    uwd_audio_freq_continuous_t *freq_continuous;
} uwd_audio_format_I_t;

#define UWD_AUDIO_SUBFRAME_SIZE(format) (((format)->bit_resolution+7) >> 3)

/* Audio Type II Format - currently not supported */

/* Audio Type III Format Type */
typedef uwd_audio_format_I_t uwd_audio_format_III_t;

/**
 * Function type:  uwd_audio_stream_cb_t
 * Description:    Application audio stream transfer completion callback, for
 *                 the uwd_audio_stream_read() and uwd_audio_stream_write()
 *                 functions.
 * Requirement:    Mandatory
 * Parameters:
 *     @app_ctx: (IN) Application context.
 *     @stream:  (IN) Pointer to an AudioStreaming interface structure.
 *     @arg:     (IN) Pointer to arguments received from uwd_audio_stream_read()
 *                    or uwd_audio_stream_write(); NULL if not required.
 *     @status:  (IN) Transfer completion status: 0 on success, otherwise
 *                    an error code.
 *
 * Return value:   None
 **/
typedef void (*uwd_audio_stream_cb_t)(uwd_audio_app_h app_ctx, 
    uwd_audio_stream_h stream, void *arg, jresult_t status, juint32_t len);

/**
 * Function type:  uwd_audio_uninit_cb_t
 * Description:    Application un-initialization callback.
 * Requirement:    Mandatory
 * Parameters:
 *     @app_ctx: (IN) Application context.
 *
 * Return value:   None
 **/
typedef void (*uwd_audio_uninit_cb_t)(uwd_audio_app_h app_ctx);

/* Audio Application Information Structure.
 * Should be available and unchanged until the uninit callback is called. */
typedef struct {
    uwd_audio_entity_t *entities;
    juint8_t             entities_num;

    uwd_audio_stream_t *streams;
    juint8_t             streams_num;

    uwd_audio_ival_t    intr_ep_ival;

    uwd_audio_uninit_cb_t uninit;   /* Mandatory */
} uwd_audio_config_t;

#endif


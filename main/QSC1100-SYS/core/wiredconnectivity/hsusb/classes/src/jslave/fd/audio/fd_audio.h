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

/* Jungo Confidential, Copyright (c) 2007 Jungo Ltd.  http://www.jungo.com */
#ifndef _FD_AUDIO_H_
#define _FD_AUDIO_H_

#include <jusb_common.h>
#include <jusb_core_fd.h>
#include <uw_device_audio.h>

#define AUDIO_CLASS              0x01
#define AUDIO_SUBCLASS_CONTROL   0x01
#define AUDIO_SUBCLASS_STREAMING 0x02

/* Class-Specific Descriptor Types */
#define CS_UNDEFINED     0x20
#define CS_DEVICE        0x21 
#define CS_CONFIGURATION 0x22 
#define CS_STRING        0x23
#define CS_INTERFACE     0x24
#define CS_ENDPOINT      0x25

/* Audio Class-Specific Endpoint Descriptor Subtypes */
#define EP_GENERAL 0x01

/* Audio Class-Specific AC Interface Descriptor Subtypes */
#define AC_SUBTYPE_UNDEFINED        0x00
#define AC_SUBTYPE_HEADER           0x01
#define AC_SUBTYPE_INPUT_TERMINAL  ((byte_t)UWD_AUDIO_ET_INPUT_TERMINAL)
#define AC_SUBTYPE_OUTPUT_TERMINAL ((byte_t)UWD_AUDIO_ET_OUTPUT_TERMINAL)
#define AC_SUBTYPE_MIXER_UNIT      ((byte_t)UWD_AUDIO_ET_MIXER_UNIT)
#define AC_SUBTYPE_SELECTOR_UNIT   ((byte_t)UWD_AUDIO_ET_SELECTOR_UNIT)
#define AC_SUBTYPE_FEATURE_UNIT    ((byte_t)UWD_AUDIO_ET_FEATURE_UNIT)
#define AC_SUBTYPE_PROCESSING_UNIT ((byte_t)UWD_AUDIO_ET_PROCESSING_UNIT)
#define AC_SUBTYPE_EXTENSION_UNIT  ((byte_t)UWD_AUDIO_ET_EXTENSION_UNIT)

/* Audio Class-Specific AS Interface Descriptor Subtypes */
#define AS_UNDEFINED       0x00
#define AS_GENERAL         0x01
#define AS_FORMAT_TYPE     0x02
#define AS_FORMAT_SPECIFIC 0x03

/* Format Type Codes */
#define AF_TYPE_UNDEFINED 0x00
#define AF_TYPE_I         0x01
#define AF_TYPE_II        0x02
#define AF_TYPE_III       0x03

#define AUDIO_DEVICE_CLASS_SPEC_RELEASE 0x0100

#define USB_FULL_SPEED_FREQ   1000    /* Hz */
#define ISOC_MAX_PACKET_FULL  1023

#define JUINT32_MAX 0xffffffff
#define JUINT16_MAX 0xffff

typedef enum {
    STATE_DISABLED = 0,
    STATE_ENABLED
} audio_state_t;

typedef enum {
    AR_ENDPOINT,
    AR_CTRL_INTERFACE,
    AR_STREAM_INTERFACE
} audio_req_type_t;

typedef struct {
    audio_req_type_t type;
    union {
        uwd_audio_entity_h entity;
        uwd_audio_stream_h stream;
    } recip;

    uwd_audio_req_code_t code;
    juint16_t value;
    juint16_t len;
    request_t *core_req;
} audio_req_t;

typedef struct {
    juint8_t str_id;
} entity_priv_t, *entity_priv_h;

typedef struct stream_buf_s {
    STAILQ_ENTRY(stream_buf_s) next;

    jresult_t              rc;
    void                  *mem;
    juint32_t              len;
    uwd_audio_stream_cb_t cb;
    void                  *arg;
} stream_buf_t;

#define STREAM_MAX_REQ_NUM 3

typedef struct fd_audio_s *fd_audio_h;

#define BYTE_PTR(ptr) ((juint8_t *)(void *)(ptr))
#define STRUCT_OFFSET(s, field) (BYTE_PTR(&s->field) - BYTE_PTR(s))

/* Stream private data */
typedef struct {
    juint8_t flags[2];
#define STREAM_FD_MEM_DESC   1

    fd_audio_h fd;
    uwd_audio_stream_h stream;

    juint8_t alt;
    juint8_t reqs;
    jbool_t  aborting;

    pipe_desc_t *pipe;
    
    STAILQ_HEAD(stream_bufs, stream_buf_s) bufs;
    juint32_t done_buf_ofs;
    juint32_t done_buf_actlen;

    struct {
        stream_buf_t *buf;
        juint32_t buf_ofs;

        request_t *req;
        juint_t req_ofs;
        juint_t req_len;
    } cur;

    juint32_t samp_freq;
    juint16_t isoc_freq;
    juint_t   audio_frame_size;
    juint16_t max_pkt;
    jint32_t  sync_frames;

    struct {
        juint32_t sum;
        juint32_t div;
        juint32_t rem;
    } average;

} stream_priv_t, *stream_priv_h;

/* Audio FD context */
typedef struct fd_audio_s {
    void             *core_ctx;
    uwd_audio_app_h  app_ctx;

    audio_state_t    state;
    audio_state_t    prev_state;
    juint8_t          flags;
#define AUDIO_APP_INIT      0x01
#define AUDIO_DESC_INIT     0x02

    uwd_audio_config_t *config;

    juint8_t ctrl_iface_index;

    juint8_t max_entity_id;
    juint8_t *entity_index;

    entity_priv_t *entity_priv;
    stream_priv_t *stream_priv;

    fd_desc_t *desc;

    audio_req_t req;

} fd_audio_t;

typedef juint8_t byte_t;
typedef juint8_t jword_t[2];

#define UWD_AUDIO_SAMP_FREQ_SIZE 3
typedef juint8_t audio_samp_freq_t[UWD_AUDIO_SAMP_FREQ_SIZE];

/* Class Specific Audio Control Interface Descriptors */

typedef __PACKED_PRE__ struct {
    byte_t bLength;
    byte_t bDescriptorType;
    byte_t bDescriptorSubtype;
    jword_t bcdADC;
    jword_t wTotalLength;
    byte_t bInCollection;
    byte_t baInterfaceNr[1];
} __PACKED_POST__ ctrl_iface_desc_t;

typedef __PACKED_PRE__ struct {
    byte_t    bLength;
    byte_t    bDescriptorType;
    byte_t    bDescriptorSubtype;
    byte_t    bTerminalID;
    jword_t    wTerminalType;
    byte_t    bAssocTerminal;
    byte_t    bNrChannels;
    jword_t    wChannelConfig;
    byte_t    iChannelNames;
    byte_t    iTerminal;
} __PACKED_POST__ input_trm_desc_t;

typedef __PACKED_PRE__ struct {
    byte_t    bLength;
    byte_t    bDescriptorType;
    byte_t    bDescriptorSubtype;
    byte_t    bTerminalID;
    jword_t    wTerminalType;
    byte_t    bAssocTerminal;
    byte_t    bSourceID;
    byte_t    iTerminal;
}  __PACKED_POST__ output_trm_desc_t;

typedef __PACKED_PRE__ struct {
    byte_t    bLength;
    byte_t    bDescriptorType;
    byte_t    bDescriptorSubtype;
    byte_t    bUnitID;
    byte_t    bSourceID;
    byte_t    bControlSize;  /* sizeof(bmaControls[0]) */
#define AUDIO_FU_CONTROL_SIZE sizeof(jword_t)
    jword_t    bmaControls[1]; /* dymamic size depends on channels number */
    byte_t    d_iFeature;
} __PACKED_POST__ feature_unit_desc_t;

typedef __PACKED_PRE__ struct {
    byte_t    bLength;
    byte_t    bDescriptorType;
    byte_t    bDescriptorSubtype;
    byte_t    bUnitID;
    byte_t    bNrInPins;
    byte_t    baSourceID[1]; /* dymamic size defined by bNrInPins */
    byte_t    d_iTerminal;
}  __PACKED_POST__ selector_unit_desc_t;

typedef __PACKED_PRE__ struct {
    byte_t    bLength;
    byte_t    bDescriptorType;
    byte_t    bDescriptorSubtype;
    byte_t    bUnitID;
    jword_t    wExtensionCode;
    byte_t    bNrInPins;
    byte_t    baSourceID[1];   /* dymamic size defined by bNrInPins */
    byte_t    d_bNrChannels;
    jword_t    d_wChannelConfig;
    byte_t    d_iChannelNames;
    byte_t    d_bControlSize;  /* = 1 */
    byte_t    d_bmControls[1];
    byte_t    d_iExtension;
}  __PACKED_POST__ extension_unit_desc_t;

/* Class Specific Audio Streaming Interface Descriptors */
typedef __PACKED_PRE__ struct {
    byte_t    bLength;
    byte_t    bDescriptorType;    
    byte_t    bDescriptorSubtype;
    byte_t    bTerminalLink;
    byte_t    bDelay;
    jword_t    wFormatTag;
} __PACKED_POST__ sif_desc_t;

/* Continuous Sampling Frequecy Descriptor */
typedef __PACKED_PRE__ struct {
    byte_t            bSamFreqType;
    audio_samp_freq_t tLowerSamFreq;
    audio_samp_freq_t tUpperSamFreq;
} __PACKED_POST__ freq_cont_desc_t;

/* Discrete Sampling Frequecy Descriptor */
typedef __PACKED_PRE__ struct {
    byte_t            bSamFreqType;
    audio_samp_freq_t tSamFreq[1];
} __PACKED_POST__ freq_discrete_desc_t;

/* Audio Type I Format Type Descriptor */
typedef __PACKED_PRE__ struct {
    byte_t    bLength;
    byte_t    bDescriptorType;
    byte_t    bDescriptorSubtype;
    byte_t    bFormatType;
    byte_t    bNrChannels;
    byte_t    bSubframeSize;
    byte_t    bBitResolution;
} __PACKED_POST__ format_I_desc_t;

/* Audio Type II Format Type Descriptor */
typedef __PACKED_PRE__ struct {
    byte_t    bLength;
    byte_t    bDescriptorType;
    byte_t    bDescriptorSubtype;
    byte_t    bFormatType;
    jword_t    wMaxBitRate;
    jword_t    wSamplesPerFrame;
} __PACKED_POST__ format_II_desc_t;

/* Audio Type III Format Type Descriptor */
typedef format_I_desc_t format_III_desc_t;

typedef __PACKED_PRE__ struct {
    byte_t    bLength;
    byte_t    bDescriptorType;
    byte_t    bDescriptorSubtype;
    byte_t    bmAttributes;
#define AI_ATTR_SAMPLING_FREQUENCY  (1 << 0)
#define AI_ATTR_PITCH               (1 << 1)
#define AI_ATTR_MAX_PACKETS_ONLY    (1 << 7)
    byte_t    bLockDelayUnits;
    jword_t    wLockDelay;
} __PACKED_POST__ audio_isoc_ep_desc_t;

typedef __PACKED_PRE__ struct {
    byte_t    bStatusType;
#define AUDIO_INTR_PENDING         (1<<7)
#define AUDIO_INTR_MEM_CHANGED     (1<<6)
#define AUDIO_INTR_ORIG_CTRL_IFACE   0
#define AUDIO_INTR_ORIG_STREAM_IFACE 1
#define AUDIO_INTR_ORIG_STREAM_EP    2
    byte_t    bOriginator;
} __PACKED_POST__ audio_intr_data_t;

#endif


#ifndef AUDAMRWBI_H
#define AUDAMRWBI_H
/*===========================================================================

      A M R - W B / W B +   I N T E R N A L   H E A D E R   F I L E

DESCRIPTION
  This header file contains all internal definitions necessary to interface
  AMR-WB/WB+ streams with AUDFMT Services.

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/audfmt/main/latest/src/audamrwbi.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/06/07    at     Added AMR-WB stream support.
09/08/06    ay     Removed the export of the internal frame length look-up
                   table.
09/04/06    ay     Initial Version.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"     /* Target specific definitions                     */
#include "customer.h"   /* Customer specific definitions                   */
#include "comdef.h"     /* Definitions for byte, word, etc.                */
#if defined(FEATURE_AMR_WB_AUDIO_DEC) || defined(FEATURE_AMR_WBPLUS_AUDIO_DEC)

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*===========================================================================

                       AMR-WB/WB+ Stream Declarations

===========================================================================*/
/****************************************************************************
  TYPES AND DEFINITIONS
****************************************************************************/

/*
** Status enumeration
*/

/* The following enum is defined to be a 32-bit unsigned integer status. A
** non-zero MSB indicates an error status.
*/
typedef enum {
  AUDAMRWB_STATUS_SUCCESS           = 0x00000000,
  AUDAMRWB_STATUS_OK_OPEN,            /* Packet is already opened    */
  AUDAMRWB_STATUS_FAILURE           = 0x80000000,
  AUDAMRWB_STATUS_ERR_NULL,           /* Unexpected NULL pointer     */
  AUDAMRWB_STATUS_ERR_BOUNDS,         /* Out of bounds memory access */
  AUDAMRWB_STATUS_ERR_OVERFLOW,       /* Memory overflow             */
  AUDAMRWB_STATUS_ERR_NOMEM,          /* Insufficient memory alloc.  */
  AUDAMRWB_STATUS_ERR_NOSPACE,        /* Out of buffer space         */
  AUDAMRWB_STATUS_ERR_Q_FULL,         /* Queue is full               */
  AUDAMRWB_STATUS_ERR_MISMATCH,       /* Resource mismatch           */
  AUDAMRWB_STATUS_ERR_BADPARAM,       /* Bad parameters              */
  AUDAMRWB_STATUS_ERR_BADSTATE,       /* Bad state                   */
  AUDAMRWB_STATUS_ERR_UNSUPPORTED,    /* Feature is not supported    */
  AUDAMRWB_STATUS_MAX               = 0xFFFFFFFF
} audamrwb_status_type;

/*
** Status check macros
*/

#define AUDAMRWB_IS_OK(status)  (((status) & AUDAMRWB_STATUS_FAILURE) == 0)
#define AUDAMRWB_IS_BAD(status) (((status) & AUDAMRWB_STATUS_FAILURE) != 0)

/*
** Transport and storage
*/

/* The AWB transport and storage frame header format */
typedef PACKED struct {
  /* Octet 1 */
  uint8 padding_1  : 1;
  uint8 padding_2  : 1;
  uint8 fqi        : 1; /* Frame quality indicator (Q) */
  uint8 frame_type : 4;
  uint8 padding_3  : 1;
} audamrwb_awb_frm_hdr_type;

#define AUDAMRWB_AWB_FRM_HDR_LEN      (sizeof(audamrwb_awb_frm_hdr_type))
#define AUDAMRWB_AWB_MAX_FRM_DATA_LEN (60)
#define AUDAMRWB_AWB_MAX_FRM_LEN      (AUDAMRWB_AWB_FRM_HDR_LEN + \
                                       AUDAMRWB_AWB_MAX_FRM_DATA_LEN)

/* The AMR-WB+ transport and storage frame header format */
typedef PACKED struct {
  /* Octet 1 */
  uint8 frame_type : 7;
  uint8 zero_1     : 1;

  /* Octet 2 */
  uint8 isf    : 5;
  uint8 zero_2 : 1;
  uint8 tfi    : 2; /* TFI=0 for storage format */
} audamrwb_awp_frm_hdr_type;

#define AUDAMRWB_AWP_FPS               (4) /* Frames per superframe */
#define AUDAMRWB_AWP_FRM_HDR_LEN       (sizeof(audamrwb_awp_frm_hdr_type))
#define AUDAMRWB_AWP_MAX_FRM_DATA_LEN  (80)
#define AUDAMRWB_AWP_MAX_SFRM_DATA_LEN (AUDAMRWB_AWP_FPS * \
                                        AUDAMRWB_AWP_MAX_FRM_DATA_LEN)
#define AUDAMRWB_AWP_MAX_SFRM_LEN      (AUDAMRWB_AWP_FRM_HDR_LEN + \
                                        AUDAMRWB_AWP_MAX_SFRM_DATA_LEN)

/* Union of AMR-WB/WB+ frame header types */
typedef union {
  audamrwb_awb_frm_hdr_type awb_hdr;
  audamrwb_awp_frm_hdr_type awp_hdr;
} audamrwb_frm_hdr_type;

/* The CMX transport packet header format */
typedef PACKED struct {
  uint16 sync_word;   /* sync_word=0xFFFF */
  uint16 num_samples; /* AMR-WB: 0 < num_samples <= 15, AMR-WB+: 0 */
  uint32 num_bytes;
} audamrwb_cmx_pkt_hdr_type;

#define AUDAMRWB_3GP_AWB_MAX_FPS      (15) /* Max. frames per sample */
#define AUDAMRWB_CMX_PKT_HDR_LEN      (sizeof(audamrwb_cmx_pkt_hdr_type))

/* The DSP decoder transport packet header format (V1.1.02) */
typedef PACKED struct {
  /* Octet 1 and 2 */
  uint16 sync_word  : 16; /* sync_word=0xFFFF               */

  /* Octet 3 */
  uint8 bfi1        :  1;
  uint8 bfi2        :  1;
  uint8 bfi3        :  1;
  uint8 bfi4        :  1;
  uint8 reserved    :  1;
  uint8 vpi         :  1; /* Virtual Packet Indicator       */
  uint8 length_high :  2; /* MSBs of the super frame length */

  /* Octet 4 */
  uint8 length_low  :  8; /* LSBs of the super frame length */

  /* Octet 5 */
  uint8 frame_type  :  7;
  uint8 zero_1      :  1;

  /* Octet 6 */
  uint8 isf         :  5;
  uint8 zero_2      :  1;
  uint8 zero_3      :  1;
  uint8 zero_4      :  1;
} audamrwb_dsp_pkt_hdr_type;

#define AUDAMRWB_DSP_PKT_HDR_LEN      (sizeof(audamrwb_dsp_pkt_hdr_type))
#define AUDAMRWB_DSP_PKT_MAX_DATA_LEN (254 * 2)
#define AUDAMRWB_DSP_PKT_MAX_FRM_LEN  (AUDAMRWB_DSP_PKT_HDR_LEN + \
                                       MAX(AUDAMRWB_AWB_MAX_FRM_DATA_LEN, \
                                           AUDAMRWB_AWP_MAX_SFRM_DATA_LEN))
#define AUDAMRWB_DSP_PKT_MAX_PKT_LEN  (AUDAMRWB_DSP_PKT_HDR_LEN + \
                                       AUDAMRWB_DSP_PKT_MAX_DATA_LEN)

/*
** CMX data parse management
*/

/* The states of CMX buffer parsing */
typedef enum {
  /* Get 1st byte of SYNC word in the CMX packet header */
  AUDAMRWB_PARSE_STATE_CMX_HDR_SYNC_1 = 0,
  /* Get 2nd byte of SYNC word in the CMX packet header */
  AUDAMRWB_PARSE_STATE_CMX_HDR_SYNC_2,
  /* Get 1st byte of num. frames word in the CMX packet header */
  AUDAMRWB_PARSE_STATE_CMX_HDR_FRAMES_1,
  /* Get 2nd byte of num. frames word in the CMX packet header */
  AUDAMRWB_PARSE_STATE_CMX_HDR_FRAMES_2,
  /* Get 4 bytes of the payload length dword in the CMX packet header */
  AUDAMRWB_PARSE_STATE_CMX_HDR_LENGTH,
  /* Recover from CMX packet header errors */
  AUDAMRWB_PARSE_STATE_CMX_DATA_ERROR,
  /* Get the 1st byte of the audio frame header */
  AUDAMRWB_PARSE_STATE_FRM_HDR_1,
  /* Get the 2nd byte of the audio frame header */
  AUDAMRWB_PARSE_STATE_FRM_HDR_2,
  /* Receive audio frame data */
  AUDAMRWB_PARSE_STATE_FRM_DATA,
  /* Write audio frame data */
  AUDAMRWB_PARSE_STATE_FRM_DATA_WRITE,
  /* Complete/continue parsing for audio frames */
  AUDAMRWB_PARSE_STATE_FRM_END,
  /* Perform deinitialization for unrecoverable errors */
  AUDAMRWB_PARSE_STATE_FAILURE,
  /* End marker for this enum type */
  AUDAMRWB_PARSE_STATE_MAX
} audamrwb_parse_state_type;

/*
** External CMX packet buffer management
*/

/* State of the CMX buffers */
typedef enum {
  AUDAMRWB_CMX_BUF_STATE_LOADING,  /* All buffers are currently loading    */
  AUDAMRWB_CMX_BUF_STATE_READY,    /* At least one buffer is usable        */
  AUDAMRWB_CMX_BUF_STATE_UNDERRUN, /* All buffers have become empty        */
  AUDAMRWB_CMX_BUF_STATE_FLUSH,    /* Flushing current and pending buffers */
  AUDAMRWB_CMX_BUF_STATE_ERROR,    /* Buffer request error                 */
  AUDAMRWB_CMX_BUF_STATE_MAX
} audamrwb_cmx_buf_state_type;

/* This structure stores the metadata information and the chunk of data read
** from the current file.
*/
typedef struct {
  boolean loading;   /* TRUE if the buffer is currently loading data */
  uint32  track_pos; /* Buffer's track position                      */
  uint8*  data_ptr;  /* Pointer to the start of data                 */
  uint32  data_len;  /* Bytes left to consume                        */
  uint32  data_idx;  /* Next offset to read relative to the data_ptr */
} audamrwb_cmx_buf_type;

/* CMX data buffer structure */
typedef struct {
  snd_af_access_type* handle;             /* Handle to get data from client */

  uint32 track_len;                       /* Total track length in bytes    */
  uint32 track_pos;                       /* Next track position to read    */
  uint32 max_read_len;                    /* Max. read len. for each buffer */

  audamrwb_cmx_buf_state_type cmx_state;  /* CMX buffering state            */
  audamrwb_cmx_buf_type       cmx_buf;    /* CMX buffer                     */
} audamrwb_cmx_ctl_type;

/*
** Internal DSP circular packet buffer management
*/

/* Buffer description */
#define AUDAMRWB_PKT_BUF_SIZE   (6 * AUDAMRWB_DSP_PKT_MAX_FRM_LEN)
#define AUDAMRWB_PKT_ALIGN_SIZE (2)

/* Packet alignment macro in terms of bytes (8 bits) */
#if (AUDAMRWB_PKT_ALIGN_SIZE == 2)
  #define AUDAMRWB_PKT_ALIGN(val, align) (((val + 1) >> 1) << 1)
#else
  #define AUDAMRWB_PKT_ALIGN(val, align) (((val + align - 1) / align) * align)
#endif /* AUDAMRWB_WORD_ALIGNED */

/* Packet type */
typedef enum {
  AUDAMRWB_PKT_IF_NONE,
  AUDAMRWB_PKT_IF_DSP_RAW,
  AUDAMRWB_PKT_IF_DSP_PSEUDORAW,
  AUDAMRWB_PKT_IF_DSP_VIRTUAL,
  AUDAMRWB_PKT_IF_MAX
} audamrwb_pkt_if_type;

/* Union of packet structures */
typedef struct {
  audamrwb_dsp_pkt_hdr_type hdr; /* Unused */
} audamrwb_pkt_if_info_dsp_raw_type;

typedef struct {
  audamrwb_dsp_pkt_hdr_type hdr;
} audamrwb_pkt_if_info_dsp_pseudoraw_type;

typedef struct {
  audamrwb_dsp_pkt_hdr_type hdr;
} audamrwb_pkt_if_info_dsp_virtual_type;

typedef union {
  audamrwb_pkt_if_info_dsp_raw_type       dsp_raw;
  audamrwb_pkt_if_info_dsp_pseudoraw_type dsp_pseudoraw;
  audamrwb_pkt_if_info_dsp_virtual_type   dsp_virtual;
} audamrwb_pkt_if_info_type;

/* Circular packet buffer structure. All fields are defined to be read-only
** to users.
*/
typedef struct {
  uint8* buf_ptr;   /* Base ptr. of the packets buffer      */
  uint32 buf_size;  /* Total buffer size in bytes           */
  uint32 buf_free;  /* Free bytes available                 */

  uint8* ready_ptr; /* Head ptr. of the ready packets       */
  uint32 ready_len; /* Length of the ready packets in bytes */

  audamrwb_pkt_if_type      pkt_if;      /* Cur. pkt. interface type */
  uint32                    pkt_max_len; /* Max. len. of cur. pkt.   */
  uint32                    pkt_align;   /* Pkt. alignment           */
  audamrwb_pkt_if_info_type pkt_if_info; /* Cur. pkt. state info.    */
} audamrwb_pkt_ctl_type;

/*
** CMX packet processing management for AMR-WB/WB+ streaming
*/

/* This is the CMX packet processing structure */
typedef struct {
  audamrwb_cmx_pkt_hdr_type hdr; /* The current CMX packet header      */
  uint16 header_read;            /* Num. of CMX pkt. hdr. bytes read   */
  uint16 data_read;              /* Num. of CMX pkt. data bytes read   */
  uint16 frame_count;            /* Total frames found in the CMX pkt. */
} audamrwb_cmx_pkt_proc_type;

/*
** Frame processing management
*/

/* This is the frame processing structure */
typedef struct {
  uint16 frame_count;            /* Num. of frms. found in a storage unit */

  audamrwb_frm_hdr_type hdr; /* The current (super)frame header       */
  uint16 header_read;            /* Num. of frm. hdr. bytes read          */
  uint16 data_read;              /* Num. of frm. data bytes read          */
  uint8  frame_data[AUDAMRWB_AWB_MAX_FRM_DATA_LEN];
                             /* The current frame data (AMR-WB only)  */
  uint8  transcoded_frame_data[AUDAMRWB_AWB_MAX_FRM_DATA_LEN];
                             /* Transcoded current frame data
                             ** (AMR_WB only)                         */
  uint16 data_len;               /* (super)frame data num. bytes expected */
} audamrwb_frm_proc_type;

/*---------------------------------------------------------------------------
** Operations
**---------------------------------------------------------------------------
*/

/* Format configuration structure */
typedef struct {
  snd_cmx_af_file_type  file_type;    /* Codec to use for playback */
  snd_sample_rate_type  sample_rate;  /* Sampling rate             */
  snd_channel_enum_type num_channels; /* Number of channels        */
} audamrwb_config_type;

/* AMR-WB/WB+ modes */
typedef enum {
  AUDAMRWB_MODE_NONE,
#ifdef FEATURE_AMR_WB_AUDIO_DEC
  AUDAMRWB_MODE_DEC_AWB_FILE,
  AUDAMRWB_MODE_DEC_AWB_STREAM,
#endif /* FEATURE_AMR_WB_AUDIO_DEC */
#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
  AUDAMRWB_MODE_DEC_AWP_STREAM,
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */
  AUDAMRWB_MODE_DEC_AWP_MAX
} audamrwb_mode_type;

/* The parser structure */
typedef struct
{
  /* General parser status */
  audamrwb_status_type status;            /* General parser status          */
  uint32               elapsed_ms;        /* Time elapsed since underrun    */
  audamrwb_config_type config;            /* Format configuration           */

  /* General transcoder/parser state machine management */
  audamrwb_parse_state_type parse_state;  /* Parser state                   */
  audamrwb_mode_type        operation;    /* Parser operation mode          */
  boolean                   need_rate;    /* Need sample rate flag          */
  boolean                   run_sm;       /* Force the state machine to run */

  /* External decoder buffer management */
  audmain_cmd_buf_type *cmd_buf;

  /* External CMX packet buffer management */
  audamrwb_cmx_ctl_type cmx_ctl;

  /* Internal circular packet buffer management */
  audamrwb_pkt_ctl_type pkt_ctl;

  /* Partial CMX packet management */
  audamrwb_cmx_pkt_proc_type cur_cmx_pkt; /* Cur. CMX packet being parsed   */

  /* Partial frame management */
  audamrwb_frm_proc_type cur_frame;       /* Cur. WB/WB+ frame being parsed */
} audamrwb_parse_ctl_type;

/****************************************************************************
   INTERNAL PROTOTYPES
****************************************************************************/

/*---------------------------------------------------------------------------
** Internal Helper Functions
**---------------------------------------------------------------------------
*/

#if ((defined(AUDAMRWB_DBG_CAPTURE_CMX_PKTS) ||   \
      defined(AUDAMRWB_DBG_CAPTURE_ADEC_PKTS)) && \
     defined(FEATURE_EFS))
/*===========================================================================

FUNCTION audamrwb_dbg_io_write

DESCRIPTION
  Creates/appends source data to a file for diagnostic purposes.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audamrwb_dbg_io_write (
  char*  file_name,
  uint8* source,
  uint32 length
);
#endif /* (AUDAMRWB_DBG_CAPTURE_CMX_PKTS || AUDAMRWB_DBG_CAPTURE_ADEC_PKTS) */
       /* && FEATURE_EFS                                                    */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_cmx_is_awb_num_frames_valid

DESCRIPTION
  Determines if the indicated number of frames value for AMR-WB streams in a
  CMX packet is valid.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - If valid.
  FALSE - Otherwise.

SIDE EFFECTS
  None

===========================================================================*/
INLINE boolean audamrwb_cmx_is_awb_num_frames_valid (
  uint16 num_frames
);

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_cmx_is_awp_num_frames_valid

DESCRIPTION
  Determines if the indicated number of frames value for AMR-WB+ streams in a
  CMX packet is valid.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - If valid.
  FALSE - Otherwise.

SIDE EFFECTS
  None

===========================================================================*/
INLINE boolean audamrwb_cmx_is_awp_num_frames_valid (
  uint16 num_frames
);

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_amr_is_wb_frame

DESCRIPTION
  Determines if the indicated frame type for an AMR-WB frame is valid.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - If valid.
  FALSE - Otherwise.

SIDE EFFECTS
  None

===========================================================================*/
INLINE boolean audamrwb_amr_is_wb_frame (
  uint8 frame_type
);

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_amr_is_mixed_frame

DESCRIPTION
  Determines if the indicated frame type for an AMR-WB/WB+ mixed-mode frame is
  valid.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - If valid.
  FALSE - Otherwise.

SIDE EFFECTS
  None

===========================================================================*/
INLINE boolean audamrwb_amr_is_mixed_frame (
  uint8 frame_type
);

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_amr_is_wbp_frame

DESCRIPTION
  Determines if the indicated frame type for an AMR-WB+ frame is valid.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - If valid.
  FALSE - Otherwise.

SIDE EFFECTS
  None

===========================================================================*/
INLINE boolean audamrwb_amr_is_wbp_frame (
  uint8 frame_type
);

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_amr_is_valid_frame

DESCRIPTION
  Determines if the indicated frame type is any of the valid frame types.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - If valid.
  FALSE - Otherwise.

SIDE EFFECTS
  None

===========================================================================*/
INLINE boolean audamrwb_amr_is_valid_frame (
  uint8 frame_type
);

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_amr_is_wb_isf

DESCRIPTION
  Determines if the indicated internal sampling frequency is valid for
  AMR-WB.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - If valid.
  FALSE - Otherwise.

SIDE EFFECTS
  None

===========================================================================*/
INLINE boolean audamrwb_amr_is_wb_isf (
  uint8 isf
);

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_amr_is_mixed_isf

DESCRIPTION
  Determines if the indicated internal sampling frequency is valid for
  AMR-WB/WB+ mixed mode.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - If valid.
  FALSE - Otherwise.

SIDE EFFECTS
  None

===========================================================================*/
INLINE boolean audamrwb_amr_is_mixed_isf (
  uint8 isf
);

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_amr_is_wbp_isf

DESCRIPTION
  Determines if the indicated internal sampling frequency is valid for
  AMR-WB+.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - If valid.
  FALSE - Otherwise.

SIDE EFFECTS
  None

===========================================================================*/
INLINE boolean audamrwb_amr_is_wbp_isf (
  uint8 isf
);

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_amr_is_valid_isf

DESCRIPTION
  Determines if the indicated internal sampling frequency is valid for any of
  the valid frame types.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - If valid.
  FALSE - Otherwise.

SIDE EFFECTS
  None

===========================================================================*/
INLINE boolean audamrwb_amr_is_valid_isf (
  uint8 isf
);

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_calc_frame_data_len

DESCRIPTION
  Calculates the number of bytes of frame data given a frame type.

DEPENDENCIES
  None

RETURN VALUE
  uint16 - The frame length in bytes.

SIDE EFFECTS
  None

===========================================================================*/
STATIC uint16 audamrwb_calc_frame_data_len (
  uint8 frame_type
);

/* <EJECT> */
/*---------------------------------------------------------------------------
** Internal CMX Buffer Functions
**---------------------------------------------------------------------------
*/

/*===========================================================================

FUNCTION audamrwb_cmx_free_data

DESCRIPTION
  Frees the data pointer of a buffer. NULL pointers are acceptable.

DEPENDENCIES
  None

RETURN VALUE
  uint8* - The new pointer of the freed buffer.

SIDE EFFECTS
  None

===========================================================================*/
STATIC uint8* audamrwb_cmx_free_data (
  audamrwb_parse_ctl_type *parse_ctl,
  uint8*                   data_ptr
);

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_cmx_free

DESCRIPTION
  Frees indicated buffer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The indicated buffer is initialized for the next data request.

===========================================================================*/
STATIC void audamrwb_cmx_free (
  audamrwb_parse_ctl_type *parse_ctl,
  audamrwb_cmx_buf_type   *buffer
);

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_cmx_is_free

DESCRIPTION
  Determines if the indicated buffer is no longer in use.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - The indicated CMX buffer is no longer in use.
  FALSE - Otherwise.

SIDE EFFECTS
  None

===========================================================================*/
INLINE boolean audamrwb_cmx_is_free (
  audamrwb_cmx_buf_type *buffer
);

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_cmx_can_recycle

DESCRIPTION
  Determines if the indicated buffer can be recycled/freed.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - The indicated CMX buffer can be recycled.
  FALSE - Otherwise.

SIDE EFFECTS
  None

===========================================================================*/
INLINE boolean audamrwb_cmx_can_recycle (
  audamrwb_cmx_buf_type *buffer
);

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_cmx_request

DESCRIPTION
  Initiates an asynchronous CMX data request for the indicated buffer.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_SUCCESS - Sent the CMX data request.
  AUDMAIN_STATUS_ERROR   - Otherwise.

SIDE EFFECTS
  None

===========================================================================*/
STATIC audmain_status_type audamrwb_cmx_request (
  audamrwb_parse_ctl_type *parse_ctl,
  audamrwb_cmx_buf_type   *buffer
);

/* <EJECT> */
/*---------------------------------------------------------------------------
** Internal Circular Packet Buffer Functions
**---------------------------------------------------------------------------
*/

/*===========================================================================

FUNCTION audamrwb_dsp_pkt_hdr_set_len

DESCRIPTION
  Sets the length field of the DSP packet header.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
INLINE void audamrwb_dsp_pkt_hdr_set_len (
  audamrwb_dsp_pkt_hdr_type* dsp_pkt_hdr,
  uint32                     length
);

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_dsp_pkt_hdr_get_len

DESCRIPTION
  Gets the length field of the DSP packet header.

DEPENDENCIES
  None

RETURN VALUE
  uint32 - The length of the packet payload.

SIDE EFFECTS
  None

===========================================================================*/
INLINE uint32 audamrwb_dsp_pkt_hdr_get_len (
  audamrwb_dsp_pkt_hdr_type* dsp_pkt_hdr
);

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_pkt_calc_base_offset

DESCRIPTION
  Calculates the distance between the tail of the indicated <pointer, length>
  pair and the base of the circular packet buffer.

  This function is primarily used in buffer wrap-around conditions for read
  and write operations.

DEPENDENCIES
  None

RETURN VALUE
  uint32 - The distance in bytes.

SIDE EFFECTS
  None

===========================================================================*/
INLINE uint32 audamrwb_pkt_calc_base_offset (
  audamrwb_pkt_ctl_type *pkt_ctl,
  uint8*                 cur_ptr,
  uint32                 length
);

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_pkt_calc_offset_ptr

DESCRIPTION
  Calculates the pointer, within the circular packet buffer, that is relative
  to the beginning of the current starting position.

  The offset value is the number of bytes relative to the current starting
  position of the circular packet buffer. The offset value must be equal or
  less than the circular packet buffer size.

DEPENDENCIES
  None

RETURN VALUE
  uint8* - The calculated pointer.

SIDE EFFECTS
  None

===========================================================================*/
STATIC uint8* audamrwb_pkt_calc_offset_ptr (
  audamrwb_pkt_ctl_type *pkt_ctl,
  uint32                 offset
);

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_pkt_calc_linear_len

DESCRIPTION
  Calculates the maximum linear length accessible without causing or
  requiring address wrap-around.

  The offset value is the number of bytes relative to the current starting
  position of the circular packet buffer. The offset value must be equal or
  less than the circular packet buffer size.

  This function is primarily used to determine the length of the head of the
  current buffer. The remaining length is part of the tail which begins from
  the base of the circular packet buffer.

DEPENDENCIES
  None

RETURN VALUE
  uint32 - The length of the head of the current buffer.

SIDE EFFECTS
  None

===========================================================================*/
STATIC uint32 audamrwb_pkt_calc_linear_len (
  audamrwb_pkt_ctl_type *pkt_ctl,
  uint32                 offset
);

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_pkt_init

DESCRIPTION
  Creates an instance of the packet control structure.

  The buf_size is the amount in bytes to dynamically allocate for the internal
  circular packet buffer.

DEPENDENCIES
  pkt_ctl - The provided structure must be zero-filled.

RETURN VALUE
  audamrwb_status_type

SIDE EFFECTS
  None

===========================================================================*/
STATIC audamrwb_status_type audamrwb_pkt_init (
  audamrwb_pkt_ctl_type *pkt_ctl,
  uint32                 buf_size
);

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_pkt_deinit

DESCRIPTION
  Destroys an instance of the packet control structure.

DEPENDENCIES
  pkt_ctl - The provided structure must initialized.

RETURN VALUE
  audamrwb_status_type

SIDE EFFECTS
  None

===========================================================================*/
STATIC audamrwb_status_type audamrwb_pkt_deinit (
  audamrwb_pkt_ctl_type *pkt_ctl
);

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_pkt_is_opened

DESCRIPTION
  Returns the status of the current working packet.

DEPENDENCIES
  None

RETURN VALUE
  boolean - TRUE if a packet is currently opened for processing.

SIDE EFFECTS
  None

===========================================================================*/
INLINE boolean audamrwb_pkt_is_opened (
  audamrwb_pkt_ctl_type *pkt_ctl
);

/*===========================================================================

FUNCTION audamrwb_pkt_is_full

DESCRIPTION
  Determines if the circular packet buffer is full.

DEPENDENCIES
  None

RETURN VALUE
  boolean - TRUE if buffer is full.

SIDE EFFECTS
  None

===========================================================================*/
INLINE boolean audamrwb_pkt_is_full (
  audamrwb_pkt_ctl_type *pkt_ctl
);

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_pkt_is_ready

DESCRIPTION
  Determines if the circular packet buffer's ready queue is not empty.

DEPENDENCIES
  None

RETURN VALUE
  boolean - TRUE if data is available for processing.

SIDE EFFECTS
  None

===========================================================================*/
INLINE boolean audamrwb_pkt_is_ready (
  audamrwb_pkt_ctl_type *pkt_ctl
);

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_pkt_get_free

DESCRIPTION
  Returns the free length of the circular packet buffer.

DEPENDENCIES
  None

RETURN VALUE
  uint32 - Number of bytes free.

SIDE EFFECTS
  None

===========================================================================*/
INLINE uint32 audamrwb_pkt_get_free (
  audamrwb_pkt_ctl_type *pkt_ctl
);

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_pkt_write

DESCRIPTION
  Writes data to the circular packet buffer.

  The offset is relative to the beginning of the current starting position of
  the circular packet buffer.

  A NULL source_ptr writes nothing to the circular packet buffer.

  The *written_len returns to the caller the amount of data written in bytes.

  When the is_atomic is TRUE, the write operation must complete entirely.

  The sum of the offset and source_len must not exceed the circular packet
  buffer size. The size of the current working packet grows larger if the sum
  of the offset and source_len exceeds causes the working packet size to
  increase.

DEPENDENCIES
  None

RETURN VALUE
  audamrwb_status_type

SIDE EFFECTS
  None

===========================================================================*/
STATIC audamrwb_status_type audamrwb_pkt_write (
  audamrwb_pkt_ctl_type *pkt_ctl,
  uint32                 offset,
  uint8*                 source_ptr,
  uint32                 source_len,
  uint32                *written_len,
  boolean                is_atomic
);

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_pkt_clear

DESCRIPTION
  Clears the entire circular packet buffer.

DEPENDENCIES
  None

RETURN VALUE
  audamrwb_status_type

SIDE EFFECTS
  None

===========================================================================*/
STATIC audamrwb_status_type audamrwb_pkt_clear (
  audamrwb_pkt_ctl_type *pkt_ctl
);

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_pkt_packet_get_len

DESCRIPTION
  Returns the current packet length.

DEPENDENCIES
  None

RETURN VALUE
  uint32 - The current packet length in number of bytes.

SIDE EFFECTS
  None

===========================================================================*/
INLINE uint32 audamrwb_pkt_packet_get_len (
  audamrwb_pkt_ctl_type *pkt_ctl
);

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_pkt_packet_get_free

DESCRIPTION
  Returns the total amount of free space left in the current packet.

  This function only accounts for the virtual amount of free space available.
  Use audamrwb_pkt_get_buf_free() to determine the actual amount of free
  space left in the circular packet buffer.

DEPENDENCIES
  None

RETURN VALUE
  uint32 - Space remaining, in bytes, of the current packet.

SIDE EFFECTS
  None

===========================================================================*/
INLINE uint32 audamrwb_pkt_packet_get_free (
  audamrwb_pkt_ctl_type *pkt_ctl
);

/* <EJECT> */
#if 0
/*===========================================================================

FUNCTION audamrwb_pkt_packet_resize

DESCRIPTION
  Resizes the current working packet to the indicated length. The actual
  resize mechanism is indicated by the packet interface type of the current
  working packet.

  The requested_len must be equal or less than the maximum size allowed per
  packet according to the packet interface type.

  The *adjusted_len returns to the caller the amount of data resized in
  bytes.

  When the is_atomic is TRUE, the resize operation must complete entirely.

DEPENDENCIES
  None

RETURN VALUE
  audamrwb_status_type

SIDE EFFECTS
  None

===========================================================================*/
STATIC audamrwb_status_type audamrwb_pkt_packet_resize (
  audamrwb_pkt_ctl_type *pkt_ctl,
  uint32                 requested_len,
  uint32                *adjusted_len,
  boolean                is_atomic
);
#endif /* 0 */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_pkt_packet_write

DESCRIPTION
  Writes data to the current working packet. The actual write mechanism is
  indicated by the packet interface type of the current working packet.

  The offset is relative to the beginning of the current working packet.

  A NULL source_ptr writes nothing to the circular packet buffer.

  The *written_len returns to the caller the amount of data written in bytes.

  When the is_atomic is TRUE, the write operation must complete entirely.

  The sum of the offset and source_len must not exceed the circular packet
  buffer size. The size of the current working packet grows larger if the sum
  of the offset and source_len exceeds causes the working packet size to
  increase.

DEPENDENCIES
  None

RETURN VALUE
  audamrwb_status_type

SIDE EFFECTS
  None

===========================================================================*/
STATIC audamrwb_status_type audamrwb_pkt_packet_write (
  audamrwb_pkt_ctl_type *pkt_ctl,
  uint32                 offset,
  uint8*                 source_ptr,
  uint32                 source_len,
  uint32                *written_len,
  boolean                is_atomic
);

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_pkt_packet_sync

DESCRIPTION
  Synchronizes the current working packet to the ready queue.

DEPENDENCIES
  None

RETURN VALUE
  audamrwb_status_type

SIDE EFFECTS
  None

===========================================================================*/
STATIC audamrwb_status_type audamrwb_pkt_packet_sync (
  audamrwb_pkt_ctl_type *pkt_ctl
);

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_pkt_packet_clear

DESCRIPTION
  Clears the current working packet.

DEPENDENCIES
  None

RETURN VALUE
  audamrwb_status_type

SIDE EFFECTS
  None

===========================================================================*/
STATIC audamrwb_status_type audamrwb_pkt_packet_clear (
  audamrwb_pkt_ctl_type *pkt_ctl
);

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_pkt_packet_create

DESCRIPTION
  Creates/reopens a new current working packet.

  The *required_len returns the number of bytes needed or allocated by the
  current working packet for the indicated packet interface type.

  The *pkt_if_info should be non-NULL if the indicated packet interface type
  is AUDAMRWB_PKT_IF_DSP_PSEUDORAW.

  The circular packet buffer begins with the ready queue followed by the
  current working packet. The ready queue can contain zero or more packets,
  inserted in FIFO order, to be consumed by the client. The current working
  packet is appended to the end of the ready queue when it becomes closed--
  closed packets should not be modified. At most one working packet exists at
  any time.

DEPENDENCIES
  None

RETURN VALUE
  audamrwb_status_type

SIDE EFFECTS
  None

===========================================================================*/
STATIC audamrwb_status_type audamrwb_pkt_packet_create (
  audamrwb_pkt_ctl_type     *pkt_ctl,
  audamrwb_pkt_if_type       pkt_if,
  audamrwb_pkt_if_info_type *pkt_if_info,
  uint32                    *required_len
);

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_pkt_packet_close

DESCRIPTION
  Closes and appends the current working packet to the end of the ready
  queue.

  The *required_len returns the number of bytes needed or allocated to close
  the current working packet.

DEPENDENCIES
  None

RETURN VALUE
  audamrwb_status_type

SIDE EFFECTS
  None

===========================================================================*/
STATIC audamrwb_status_type audamrwb_pkt_packet_close (
  audamrwb_pkt_ctl_type *pkt_ctl,
  uint32                *required_len
);

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_pkt_ready_read

DESCRIPTION
  Retrieves the head and tail buffers to the ready queue.

  This function is used by the client to retrieve data ready for processing.

DEPENDENCIES
  None

RETURN VALUE
  audamrwb_status_type

SIDE EFFECTS
  None

===========================================================================*/
STATIC audamrwb_status_type audamrwb_pkt_ready_read (
  audamrwb_pkt_ctl_type *pkt_ctl,
  uint8*                *head_ptr,
  uint32                *head_len,
  uint8*                *tail_ptr,
  uint32                *tail_len
);

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_pkt_ready_sync

DESCRIPTION
  Synchronizes the current starting position of the circular packet buffer to
  reflect the amount of data read from the ready queue.

  This function is used by the client to notify the amount of data consumed
  from the ready queue.

DEPENDENCIES
  None

RETURN VALUE
  audamrwb_status_type

SIDE EFFECTS
  None

===========================================================================*/
STATIC audamrwb_status_type audamrwb_pkt_ready_sync (
  audamrwb_pkt_ctl_type *pkt_ctl,
  uint32                 consumed_len
);

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_pkt_ready_clear

DESCRIPTION
  Clears the ready queue.

DEPENDENCIES
  None

RETURN VALUE
  audamrwb_status_type

SIDE EFFECTS
  None

===========================================================================*/
STATIC audamrwb_status_type audamrwb_pkt_ready_clear (
  audamrwb_pkt_ctl_type *pkt_ctl
);

/* <EJECT> */
/*---------------------------------------------------------------------------
** Internal Parser Functions
**---------------------------------------------------------------------------
*/

/*===========================================================================

FUNCTION audamrwb_parse_init

DESCRIPTION
  Initializes the internal resources and behavior of the parser.

DEPENDENCIES
  None

RETURN VALUE
  audamrwb_status_type

SIDE EFFECTS
  None

===========================================================================*/
STATIC audamrwb_status_type audamrwb_parse_init (
  audamrwb_parse_ctl_type *parse_ctl,
  audamrwb_mode_type       mode
);

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_parse_reset

DESCRIPTION
  Resets the parser state to starting values.

DEPENDENCIES
  None

RETURN VALUE
  audamrwb_status_type

SIDE EFFECTS
  None

===========================================================================*/
STATIC audamrwb_status_type audamrwb_parse_reset (
  audamrwb_parse_ctl_type *parse_ctl
);

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_cmx_buf_consume

DESCRIPTION
  Consumes and stores the indicated number of bytes of data from the source
  buffer to a destination buffer.

  This function is primarily used to consume CMX buffers during parsing. The
  caller must ensure that there is enough data remaining to consume.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
STATIC void audamrwb_cmx_buf_consume (
  uint8*  *cmx_buf,
  uint32* *cmx_buf_len,
  uint8*   dest_ptr,
  uint32   length
);
#ifdef FEATURE_AMR_WB_AUDIO_DEC

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_transcode_awb_storage

DESCRIPTION
  AMR-WB storage parser.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
STATIC void audamrwb_transcode_awb_storage (
  audamrwb_parse_ctl_type *parse_ctl,
  uint8*                   cmx_buf,
  uint32                  *cmx_buf_len
);

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_transcode_awb_stream

DESCRIPTION
  AMR-WB stream parser.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
STATIC void audamrwb_transcode_awb_stream (
  audamrwb_parse_ctl_type *parse_ctl,
  uint8*                   cmx_buf,
  uint32                  *cmx_buf_len
);
#endif /* FEATURE_AMR_WB_AUDIO_DEC */

/* <EJECT> */
#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
/*===========================================================================

FUNCTION audamrwb_transcode_awp_stream

DESCRIPTION
  AMR-WB+ stream parser.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
STATIC void audamrwb_transcode_awp_stream (
  audamrwb_parse_ctl_type *parse_ctl,
  uint8*                   cmx_buf,
  uint32                  *cmx_buf_len
);
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_transcode_stream

DESCRIPTION
  Parses CMX interface packets, from the indicated CMX buffer, to DSP
  interface packets.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
STATIC void audamrwb_transcode_stream (
  audamrwb_parse_ctl_type *parse_ctl,
  audamrwb_cmx_buf_type   *buffer
);

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_transfer_cmx2adec

DESCRIPTION
  Transcodes the received CMX buffers to the currently configured stream type.
  Requests for more data from CMX as needed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
STATIC audmain_status_type audamrwb_transfer_cmx2adec (
  audamrwb_parse_ctl_type *parse_ctl,
  uint32                  *time_remain_ptr
);

/* <EJECT> */
/*---------------------------------------------------------------------------
** Internal Support Functions
**---------------------------------------------------------------------------
*/

/*===========================================================================

FUNCTION audamrwb_save_playback_config

DESCRIPTION
  This function saves and verifies that the playback configuration is
  supported.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - If the playback configuration is supported.
  FALSE - Otherwise.

SIDE EFFECTS
  audamrwb_info is setup.

===========================================================================*/
STATIC boolean audamrwb_save_playback_config (
  audamrwb_parse_ctl_type         *parse_ctl,
  snd_ext_speech_format_spec_type *input_spec
);
#endif /* FEATURE_AMR_WB_AUDIO_DEC || FEATURE_AMR_WBPLUS_AUDIO_DEC */
#endif /* AUDAMRWBI_H */

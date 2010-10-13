#ifndef AUDQCPI_H
#define AUDQCPI_H
/*===========================================================================

               Q C P   A U D I O   F O R M A T   I N T E R N A L
                          H E A D E R    F I L E

DESCRIPTION
  This header file contains all the internal definitions for 
  the QCP audio format module.

Copyright (c) 2001 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.
   
  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/audfmt/main/latest/src/audqcpi.h#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/07/09    sj     Removed featurization from audqcp_parse_ctl_type structure
                   declaration.
12/29/08    sj     Added .awb file format support
08/04/05    sp     Added support for mixing of qcp audio and voice call.
07/19/05    pl     Added error_status to audqcp_parse_ctl_type.
01/22/04    lg     Added get_time functionality
12/08/03    aw     Added bitmask to indicate the current status.
10/16/03  aw/st    Added restart_flag in audqcp_parse_ctl_type.
04/17/03    sm     Added AMR file format support.
07/25/02    st     Added support for flushing.
09/03/01  st/aw    Modified code to support playback ff/rewind functions;
                   added code to support QCP support functions.
04/04/01    st     created file

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"             /* Target specific definitions            */
#include "comdef.h"             /* Definitions for byte, word, etc.       */

#ifdef FEATURE_QCP
#include "voc.h"                /* Vocoder definitions and interface      */
#include "qcpsup.h"             /* QCP Format utilities definitions       */

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/* This bitmask is used to indicate the status of the QCP parser/player.
*/
typedef enum {
  AUDQCP_MASK_RESTART      = 0x0001, /* Restart QCP playback              */
  AUDQCP_MASK_FAILURE      = 0x0002  /* Receive failure status            */
} audqcp_status_mask_type;

/* This structure hold all the data necessary to control a single prompt
** buffer.
*/
typedef struct {
  uint8               *track_buf;     /* Current data buffer being used   */
  uint32              start_frame;    /* Frame number at start of Prompt1 */
  voc_pb_header_type  boarder;        /* Single frame prompt              */
  voc_pb_header_type  prompt;         /* Prompt                           */
} audqcp_prompt_type;

typedef enum {
  AUDQCP_STATUS_NONE,
  AUDQCP_STATUS_PARSE_HEAD,
  AUDQCP_STATUS_CALC_TIME,
  AUDQCP_STATUS_DONE_TIME,
  AUDQCP_STATUS_PLAY
} audqcp_status_enum_type;

/* This structure contains all the information on a particular track
** that is being parsed.
*/
typedef struct {
  snd_af_access_type  *audqcp_handle; /* Info to callback client          */
  qcpsup_format_enum_type
                      format;         /* Format of QCP data               */
  voc_pb_dir_type     direction;      /* Path to play QCP out of          */
  uint32              track_start;    /* Position in file of this track   */
  uint32              track_length;   /* Length, in bytes, of this track  */
  uint32              file_location;  /* Location in file being processed */
  uint8               frame[3][65];   /* Buffer for boundary frames       */
  uint32              frame_index;    /* Index of boundary frame buffers  */
  uint32              index;          /* Current index into partial frame */
  uint32              remain;         /* Bytes remain to complete frame   */
  uint32              seek_ms;        /* Forward seek time                */
  audqcp_prompt_type  prompt1;        /* First ping-pong prompt buffer    */
  audqcp_prompt_type  prompt2;        /* Second ping-pong prompt buffer   */
  audqcp_prompt_type  *curr_prompt;   /* Current prompt used to time calc */
  boolean             wideband_flag;  /* TRUE if AMR-WB data              */
  uint8               num_channels;   /* Number of AMR channels           */
  uint32              status_flag;    /* status of QCP playback           */
  uint32              num_frames;     /* Total # of frames from vrat chunk */
  audqcp_status_enum_type  status;		  /*Current status of QCP data        */
  boolean             in_call;        /* If playback mixed with voice call*/
  audmain_status_type error_status;   /* Indicate the parse error         */
} audqcp_parse_ctl_type;

/* This structure is used to control the data request callbacks
*/
typedef struct {
  audqcp_prompt_type   *skip;    /* Skip prompt                              */
  audqcp_prompt_type   *first;   /* Next prompt to receive data              */
  audqcp_prompt_type   *second;  /* Prompt to request data after next buffer */
  audmain_cmd_buf_type *cmd_buf; /* Command buffer to queue up new prompts   */
} audqcp_buf_mgmt_type;

/* This defines the number of milliseconds in one frame.
*/
#define AUDQCP_FRAME_TIME    20

/* <EJECT> */
/*---------------------------------------------------------------------------
** AUDQCP local function prototypes
**---------------------------------------------------------------------------
*/
/*===========================================================================

FUNCTION audqcp_func_req

DESCRIPTION
  This function does the get vrat or data or cnfg processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void audqcp_func_req (
  snd_af_access_type    *handle,
  uint8                 *buffer,
  uint32                position,
  uint32                buf_len
);

/* <EJECT> */
/*===========================================================================

FUNCTION audqcp_chunk_req

DESCRIPTION
  This function does the get/set label/text processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void audqcp_chunk_req (
  snd_af_access_type *handle,
  uint8              *buffer,
  uint32             position,
  uint32             buf_len
);

#endif /* FEATURE_QCP */
#endif /* AUDQCPI_H */

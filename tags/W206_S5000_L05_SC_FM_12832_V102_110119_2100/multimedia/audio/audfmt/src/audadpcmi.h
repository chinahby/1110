#ifndef AUDADPCMI_H
#define AUDADPCMI_H
/*===========================================================================

              A D P C M   A U D I O   F O R M A T   I N T E R N A L
                          H E A D E R    F I L E

DESCRIPTION
  This header file contains all the internal definitions for 
  the ADPCM audio format module.

Copyright (c) 2003-2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.
   
  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/audfmt/main/latest/src/audadpcmi.h#1 $ $DateTime: 2008/04/24 13:56:20 $ $Author: yuanz $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/10/07    st     Modified code to support an additional reserve data buffer.
06/26/06    pl     Added new AUDADPCM_MASK and redefined 
                   audadpcm_track_status_enum_type.
07/19/05    pl     Added error_status parameter to 
                   audadpcm_track_status_enum_type.
06/03/05    st     Added num_frames parameter to audpcm_rec_ctl_type.
08/26/04    st     Added overflow_offset_ms to audadpcm_parse_ctl_type.
03/22/04    st     Modified code to support PCM playback and recording.
01/22/04    lg     Added get_time functionality
12/02/03    sg     Added support for linear PCM playback.
03/10/03    st     Removed AUDADPCM_MASK_BUF2_PENDING.
02/19/03    aw     Created file.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"             /* Target specific definitions            */
#include "comdef.h"             /* Definitions for byte, word, etc.       */

#ifdef FEATURE_WAVE_PB
#include "qcpsup.h"             /* ADPCM Format utilities definitions     */

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/* These defines are used for the ADPCM buffer. The ADPCM max frame length
** refers to the amount of space to leave free in each buffer in order to
** conform to DSP interface for ADPCM.
*/
#define AUDADPCM_BUF_LENGTH       1000
#define AUDADPCM_FRAME_LENGTH_MAX 164

/* This bitmask is used to indicate the status of the ADPCM parser/player.
*/
typedef enum {
  AUDADPCM_MASK_INIT_DONE     = 0x0001, /* Initialization done               */
  AUDADPCM_MASK_DSP_RESET     = 0x0002, /* Do DSP reset                      */
  AUDADPCM_MASK_BUF_PENDING   = 0x0004, /* Receive buffer pending            */
  AUDADPCM_MASK_COPY_PENDING  = 0x0008, /* Buffer copy pending               */
  AUDADPCM_MASK_AUX_FUNC      = 0x0010, /* Doing aux function, no DSP access */
  AUDADPCM_MASK_REPOSITION    = 0x0020, /* Repositioning data pointer        */
  AUDADPCM_MASK_BUFF_ERR_SENT = 0x0040, /* Buffer underflow status sent      */
  AUDADPCM_MASK_FAILURE       = 0x0080, /* Receive failure status            */
  AUDADPCM_MASK_DONE_PENDING  = 0x0100,
  AUDADPCM_MASK_USE_RESERVE   = 0x0200  /* Use reserve data buffer           */
} audfadec_status_mask_type;

/* This enumerated type is used to identify the two pingpong 
** data buffers.
*/
typedef enum {
  AUDADPCM_STATUS_NONE   = 0,
  AUDADPCM_STATUS_TRACK1 = 0x1,    /* Currently using track #1 data buffer */
  AUDADPCM_STATUS_TRACK2 = 0x2,    /* Currently using track #2 data buffer */
} audadpcm_track_status_enum_type;

/* This structure contains the information for one buffer of data.
*/
typedef struct {
  uint8    *buffer;      /* Pointer to data buffer         */
  uint32   index;        /* Current index into data buffer */
  uint32   length;       /* Length of data buffer in bytes */
} audadpcm_track_ctl_type;

/* This structure contains all the information on the ADPCM file
** that is being parsed.
*/
typedef struct {
  snd_af_access_type       *handle;        /* Info to callback client       */
  uint32                   status_flag;    /* Status of ADPCM parser/player */
  audmain_status_type      error_status;   /* Indicate the parse error      */
  qcpsup_adpcm_format_type format;         /* Format of ADPCM data          */
  boolean                  signed_samples; /* Format of PCM samples         */
  audadpcm_track_ctl_type  track1;         /* Buffer one of ADPCM data      */
  audadpcm_track_ctl_type  track2;         /* Buffer two of ADPCM data      */
  audadpcm_track_ctl_type  reserve;        /* Reserve data buffer           */

  /* This status indicates which data buffer is current */
  audadpcm_track_status_enum_type track_status;

  /* This status indicates which data buffer is currently locked */
  audadpcm_track_status_enum_type track_inuse;

  /* Total time to play file */
  uint32 time_ms;

  uint32  start_position;            /* Start position of ADPCM data          */
  uint32  total_length;              /* Total length of ADPCM data            */
  uint32  data_length;               /* Length of ADPCM data in bytes         */
  uint32  file_location;             /* Location in file being processed      */
  uint32  frame_size;                /* Size in bytes of 10ms frame           */
  uint32  block_length;              /* Length of ping-pong buffers           */

  uint32  data_rate;                 /* Bitrate used for seeking operations   */
  uint32  rate_offset;               /* Offset multiplier for data_rate       */
  uint32  overflow_offset;           /* Overflow value with offset factor     */
  uint32  overflow_rate;             /* Overflow value for data_rate          */
  uint32  overflow_offset_ms;        /* Overflow value for offset factor only */
} audadpcm_parse_ctl_type;
#endif /* FEATURE_WAVE_PB */

#ifdef FEATURE_PCM_REC
/* This enum type contains bitmask values for status of PCM recording
*/
typedef enum {
  AUDPCM_MASK_ENABLED       = 0x0001, /* PCM recording is enabled          */
  AUDPCM_MASK_FRAME_DROPPED = 0x0002, /* PCM recording frame dropped state */
  AUDPCM_MASK_INTERRUPTED   = 0x0004, /* PCM recording interrupted state   */
  AUDPCM_MASK_PAUSED        = 0x0008  /* PCM recording paused state        */
} audpcm_status_mask_enum_type;

/* This structure contains all information for PCM recording session
*/
typedef struct {
  uint32                       status;           /* Status of PCM recording   */
  uint32                       num_frames;       /* Number of frames recorded */
  uint32                       running_index;    /* Current index of file     */
  uint32                       file_size;        /* Size of PCM file          */
  uint8                        *current_buf;     /* Current PCM record buff   */
  uint32                       size_current_buf; /* Size of current buffer    */
  uint32                       index_current_buf;/* Index into current buff   */
  uint8                        *reserve_buf;     /* Reserve recording buffer  */
  uint32                       size_reserve_buf; /* Size of reserve buffer    */
  snd_pcm_rec_cb_func_ptr_type cb_func;          /* Client callback function  */
  const void                   *client_data;     /* Client data ptr           */
} audpcm_rec_ctl_type;
#endif /* FEATURE_PCM_REC */
#endif /* AUDADPCMI_H */

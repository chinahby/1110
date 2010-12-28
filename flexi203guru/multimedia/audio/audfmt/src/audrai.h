#ifndef AUDRAI_H
#define AUDRAI_H
/*===========================================================================

        R E A L A U D I O   I N T E R N A L   H E A D E R   F I L E

DESCRIPTION
  This header file contains all internal definitions necessary to interface
  RealAudio streams with AUDFMT Services.

Copyright (c) 2003 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
   
  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/audfmt/main/latest/src/audrai.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/19/05    pl     Added error_status to audra_parse_ctl_type.
11/23/03    sg     Initial Version.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"     /* Target specific definitions                     */
#include "customer.h"   /* Customer specific definitions                   */
#include "comdef.h"     /* Definitions for byte, word, etc.                */
#ifdef FEATURE_REAL_AUDIO

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*===========================================================================

                       RealAudio Stream Declarations

===========================================================================*/
/* Info that is static for a given song
*/
typedef struct {
  /* Fixed header - doesn't change from frame to frame */
  snd_ra_decoder_mode_type      decoder_mode;  /* Decoder (Gecko) mode     */
  snd_sample_rate_type          sample_rate;   /* Sampling rate            */
  snd_ra_channel_type           num_channels;  /* Number of channels       */
  snd_ra_samples_per_frame_type samples_per_frame; /* Samples per frame    */
  uint16                        num_regions;   /* Number of regions        */
  uint16                        bits_per_frame; /* Number of bits per frame*/
  uint16                        coupling_start; /* Stereo coupling region  */
  uint16                        coupling_quant; /* Coupling quantization   */
  uint16                        frame_erasure;  /* Frame erasure flag      */
} audra_info_type;


/* State of the ping-pong buffers */
typedef enum {
  AUDRA_DATA_STATE_LOADING, /* Both ping-pong buffers are loading             */
  AUDRA_DATA_STATE_READY,   /* Data ready, other buffer might still be loading*/
  AUDRA_DATA_STATE_FLUSH,   /* Flushing current data, and any pending Read ops*/
  AUDRA_DATA_STATE_ERROR    /* Data error                                     */
} audra_data_state_type;

/* This structure contains all the information on a particular file
** that is being parsed.
*/
/* Input data buffer */
typedef struct {
  uint8               *data;     /* Data buffer                      */
  uint32              start;     /* Position in file of buffer       */
  uint16              length;    /* Buffer data length               */
  boolean             loading;   /* TRUE == Buffer is being loaded   */
} audra_buf_type;

/* This structure contains all the information on a particular file
** that is being parsed. Two ping-pong style buffers are used, so as to allow
** one buffer to be loaded, while the other is being parsed.
*/
typedef struct {
  snd_af_access_type     *handle;        /* Info to callback client          */

  audra_buf_type         buf1;           /* ping-pong buffer 1               */
  audra_buf_type         buf2;           /* ping-pong buffer 2               */

  audra_buf_type         *curr_buf;      /* buffer being currently used      */
  uint32                 file_location;  /* Location in file being processed */
  uint32                 file_len;       /* File Length                      */
  uint32                 track_length;
  audra_data_state_type  state;
  audmain_status_type    error_status;
} audra_parse_ctl_type;

/* <EJECT> */
/*---------------------------------------------------------------------------
** Internal Function Prototypes
**---------------------------------------------------------------------------
*/

#endif /* FEATURE_REAL_AUDIO */
#endif /* AUDRAI_H */

#ifndef AUDIMY_H
#define AUDIMY_H
/*===========================================================================

           A U D I O   F O R M A T   I M E L O D Y   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface iMelody
  files with the Audio Format modules.

Copyright (c) 2002 - 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/audfmt/main/latest/src/audimy.h#1 $ $DateTime: 2008/04/24 13:56:20 $ $Author: yuanz $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/13/06    ay     Added support for iMelody pausing; also added led,
                   vibration, and backlight state information to
                   "audimy_pp_ctl".
07/19/05    pl     Added error_status to audimy_pp_ctl_type
01/22/04    lg     Added get_time functionality
11/05/02    sg     Initial version
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"     /* Target specific definitions                     */
#include "customer.h"   /* Customer specific definitions                   */
#include "comdef.h"     /* Definitions for byte, word, etc.                */

#ifdef FEATURE_IMELODY
#include "audmaini.h"   /* Audio Format Data definitions and protos        */

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*===========================================================================

                       iMelody HEADER STRUCTURE DECLARATIONS

===========================================================================*/

/* Maximum length of each line entry in an iMelody file */
#define AUDIMY_MAX_LINE_LENGTH 76

/* iMelody information structure, including the header, and all calculated data
*/
typedef struct {
  byte    format;
  char    name[AUDIMY_MAX_LINE_LENGTH];
  char    composer[AUDIMY_MAX_LINE_LENGTH];
  uint16  beat;
  byte    style;
  byte    volume;
  uint32 time_ms;  /* Total time for the file*/
} audimy_info_type;

/* Input data buffer */
typedef struct {
  uint8               *data;     /* Data buffer                      */
  uint32              start;     /* Position in file of buffer       */
  uint16              length;    /* Buffer data length               */
  boolean             loading;   /* TRUE == Buffer is being loaded   */
} audimy_buf_type;

/* This structure contains all the information on a particular iMelody file
** that is being parsed. Two ping-pong style buffers are used, so as to allow
** one buffer to be loaded, while the other is being parsed.
*/
typedef struct {
  snd_af_access_type  *handle;        /* Info to callback client          */

  uint32              file_len;       /* File Length                      */
  uint32              file_location;  /* Location in file being processed */

  audimy_buf_type     *curr_buf;      /* buffer being currently used      */
  audimy_buf_type     buf1;           /* ping-pong buffer 1               */
  audimy_buf_type     buf2;           /* ping-pong buffer 2               */

  audmain_status_type error_status;   /* Error Status encountered         */

  boolean             led_en;         /* LED enable flag                  */
  boolean             vib_en;         /* Vibration enable flag            */
  boolean             backlight_en;   /* Backlight enable flag            */
} audimy_pp_ctl_type;

/* <EJECT> */
/*---------------------------------------------------------------------------
** AudIMY Public Function Prototypes
**---------------------------------------------------------------------------
*/
/* <EJECT> */
/*===========================================================================

FUNCTION audimy_parse_head

DESCRIPTION
  This function parses the imy header for relevant infomation and initializes
  the data structures necessary to play the file.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_SUCCESS if successful

SIDE EFFECTS

===========================================================================*/
extern audmain_status_type audimy_parse_head (
  uint8                       *buffer,     /* CMX Input buffer             */
  snd_af_access_type          *handle,     /* CMX Data handle              */
  audmain_parse_ctl_type      *parse_ctl   /* data buffer info             */
);

/* <EJECT> */
/*===========================================================================

FUNCTION audimy_set_instrument

DESCRIPTION
  Sets the instrument (default is Piano).

DEPENDENCIES
  The instrument setting takes effect only if SND_METHOD_MIDI is used.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void audimy_set_instrument (
  word instrument
);

/* <EJECT> */
/*===========================================================================

FUNCTION audimy_get_data_cb_req

DESCRIPTION
  Callback function which handles the client's response to a Data request.

DEPENDENCIES
  audfmt_buf_mgmt

RETURN VALUE
  TRUE if the data was successfully obtained

SIDE EFFECTS
  audfmt_buf_mgmt

===========================================================================*/
extern boolean audimy_get_data_cb_req (
  uint8  **data,
  uint8  *buf,
  uint32 length,
  uint32 pos
);

/* <EJECT> */
/*===========================================================================

FUNCTION audimy_get_sample_rate

DESCRIPTION
  This function returns the sampling rate of the current imy song.

DEPENDENCIES

RETURN VALUE
  sample rate of the last imy file parsed

SIDE EFFECTS
  None

===========================================================================*/
extern snd_sample_rate_type audimy_get_sample_rate (
  audmain_parse_ctl_type  *parse_ctl
);

/* <EJECT> */
/*===========================================================================

FUNCTION audimy_reset

DESCRIPTION
  This function resets internal iMelody buffers after a song is unloaded.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  imy_info, imy_frame_data_found

===========================================================================*/
extern void audimy_reset ( void );

/* <EJECT> */
/*===========================================================================

FUNCTION audimy_parse

DESCRIPTION
  Parses the data file and generates equivalent MIDI commands.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_DONE    - done parsing
  AUDMAIN_STATUS_SUCCESS - sucessfully parsed tokens
  AUDMAIN_STATUS_ERROR   - Error in parsed data.

SIDE EFFECTS
  None

===========================================================================*/
extern audmain_status_type audimy_parse (
  audmain_parse_ctl_type  *parse_ctl,
  audmain_cmd_buf_type    *cmd_buf,
  uint32                  *time_remain
);

/* <EJECT> */
/*===========================================================================

FUNCTION audimy_age

DESCRIPTION
  If we have finished playing the current note, it issues the command to next
  to play the next note.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void audimy_age (
  audmain_parse_ctl_type  *parse_ctl,
  uint32                  *elapsed_time,
  uint32                  *adjusted_time
);

/* <EJECT> */
/*===========================================================================

FUNCTION audimy_do_time

DESCRIPTION
 Calculates the time in ms for playing the file.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_DONE    - Success
  AUDMAIN_STATUS_PENDING - Pending
  AUDMAIN_STATUS_ERROR

SIDE EFFECTS
  audimy_info.time_ms
===========================================================================*/
extern audmain_status_type audimy_do_time (
  audmain_parse_ctl_type  *parse_ctl,
  uint32                  *elapsed_time
);

/* <EJECT> */
/*===========================================================================

FUNCTION audimy_pause

DESCRIPTION
  This function pauses the current iMelody playback.

  In the paused state audimy_age() skips MIDI aging and processing.

  audimy_pause() causes pausing/resuming to take effect on the iMelody
  playback and vibration generators.

DEPENDENCIES
  Uses audimy_current_volume to retrieve the current parsed volume level.
  Uses audmain_audimy_pause() to pause/resume the iMelody sequence.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void audimy_pause (
  audmain_parse_ctl_type *parse_ctl,
  boolean                pause_flag
);
#endif /* FEATURE_IMELODY */
#endif /* AUDIMY_H */


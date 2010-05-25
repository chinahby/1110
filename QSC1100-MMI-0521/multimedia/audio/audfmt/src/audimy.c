/* linesize(132)
** pagesize(60)
** title("Dual Mode Subscriber Station")
** subtitle("Audfmt iMelody Services")
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             A U D I O   F O R M A T s   I M E L O D Y   S E R V I C E S

GENERAL DESCRIPTION
  This module contains AudIMY services software.
  Refer to the IrDA's iMelody 1.2 standard for details.

EXTERNALIZED FUNCTIONS
  audimy_parse_head
    Parses the imy header for relevant infomation.
  audimy_get_data_cb_req
    Callback function which handles the response to a Data request.
  audimy_reset
    Resets internal iMelody buffers after a song is unloaded.
  audimy_parse
    Parses the data file and generates equivalent MIDI commands.
  audimy_age
    If we have finished playing the current note, it issues the command to next
    to play the next note.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright(c) 2002 - 2006 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/audfmt/main/latest/src/audimy.c#1 $ $DateTime: 2008/04/24 13:56:20 $ $Author: yuanz $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/31/07   ss/sg   Removed the code which aborts playback for iMeoldy files 
                   with spaces in header and melody content. 
                   Fixed CR_124069(SR_956997)  
02/13/06    ay     Added support for iMelody pausing; also added led,
                   vibration, and backlight state information to
                   "audimy_pp_ctl".
12/22/05  ay/sk    Fixed rest note duration to apply to the loop time.
                   Fixed data error caused by skipping the last buffer.
09/15/05    ay     Added support for iMelody 1.0 format.
                   Added support for handling UNIX-style text formatting.
                   Fixed a potential buffer overflow with carriage returns.
                   Fixed a buffer overflow for empty RHS content.
                   Modified to handle (non-standard) unbalanced parenthesis
                   in loops.
            sg     Modified to handle (non-standard) un-paranthesised loop.
08/31/05    sm     Fixed buffer overflow condition in audimy_atoi().
08/19/05    pl     Modified audimy_do_time() to handle all types of error status
                   returned by audimy_parse(). SR 760011
07/19/05    pl     Added the support for setting the error status at the point
                   where the error is encountered. The error status would
                   eventually be relayed back to the upper layer to provide
                   more information for the aborted playback.
07/08/05    pl     Modified audimy_do_file_spec() to set supported operation
                   bit mask.
08/11/04    st     Updated to use new AVS midi interface.
03/05/04    sp     Fixed bug in get_time, so that it'll exit the infinite loop
                   if parsing fails due to invalid data.
01/22/04    lg     Added get_time functionality
10/16/03  aw/st    Modified code to ignore AUDMAIN_MASK_CMD_BUFF_ERROR flag in
                   audimy_age().
10/01/03    sg     Modified to not increment MIDI cmd buffer index for non-MIDI
                   commands like 'ledon' in audimy_age().
01/05/03    sg     Added input length checking to audimy_atoi(). Fixed
                   audimy_process_note() so it converts the notes to the
                   correct MIDI tones.
11/05/02    sg     Initial version
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "target.h"             /* Target specific definitions             */
#include "comdef.h"             /* Common data type definitions            */
#include "customer.h"           /* Customer specific definitions           */
#ifdef FEATURE_IMELODY
#include "msg.h"                /* Message logging/reporting services      */
#include "err.h"                /* Error logging/reporting services        */
#include <string.h>             /* For strncmp()                           */
#include <memory.h>             /* Memory manipulation routines            */
#include <stdlib.h>             /* For strtol()                            */
#include <ctype.h>              /* For toupper()                           */

#include "audimy.h"             /* iMelody typedefs and prototypes         */

/* <EJECT> */
/*---------------------------------------------------------------------------
** AUDIMY Macro and Type Definitions
**---------------------------------------------------------------------------
*/
#define AUDIMY_MAX_CMD_BUF_SIZE   255

/* Translate iMelody volume to a Qsynth volume
 * IMY range: 0-15; Qsynth range 0-127.
 * We will set IMY 0 to be Qsynth 37 and divide up (127-37) into 15
 * levels.
 */
#define AUDIMY_CALC_VOLUME(x)  (x)? (37 + (6 * (x))): 0

#define AUDIMY_MIDI_RESET           0xFF
#define AUDIMY_MIDI_CONTROL_CHANGE  0xB0
#define AUDIMY_MIDI_NOTE_ON         0x90
#define AUDIMY_MIDI_NOTE_OFF        0x80
#define AUDIMY_MIDI_PROGRAM_CHANGE  0xC0

/* State of the ping-pong buffers */
typedef enum {
  AUDIMY_DATA_LOADING, /* Both ping-pong buffers are loading                  */
  AUDIMY_DATA_READY,   /* Data ready, but there may be 1 buffer still loading */
  AUDIMY_DATA_FLUSH,   /* Flushing current data, and any pending Read ops.    */
  AUDIMY_DATA_ERROR    /* Data error                                          */
} audimy_data_state_type;

/* iMelody command definition */
typedef enum {
  AUDIMY_MEL_RESET,      /* Reset the MIDI player              */
  AUDIMY_MEL_PROGRAM,    /* MIDI program change                */
  AUDIMY_MEL_BACKLIGHT,  /* Backlight ON/OFF command           */
  AUDIMY_MEL_LED,        /* LED ON/OFF command                 */
  AUDIMY_MEL_VIBRATOR,   /* Vibrator ON/OFF command            */
  AUDIMY_MEL_VOLUME,     /* Volume change command              */
  AUDIMY_MEL_NOTE_ON,    /* MIDI note ON command               */
  AUDIMY_MEL_NOTE_OFF,   /* MIDI note OFF command              */
  AUDIMY_MEL_END,        /* Last command issued (Done playing) */
  AUDIMY_MEL_MAX
} audimy_mel_type;

/* length of each command */
const byte audimy_cmd_len[] = {
  1,  /* RESET     */
  3,  /* PROGRAM   */
  1,  /* BACKLIGHT */
  1,  /* LED       */
  1,  /* VIBRATOR  */
  4,  /* VOLUME    */
  5,  /* NOTE_ON   (Includes duration) */
  5,  /* NOTE_OFF  (Includes duration) */
  0,  /* END       */
};

/* <EJECT> */
/*---------------------------------------------------------------------------
** AUDIMY Data Definitions
**---------------------------------------------------------------------------
*/
const byte AUDIMY_MAX_TOKEN_SIZE  = 11;
const char AUDIMY_CARRIAGE_RETURN = '\r';
const byte AUDIMY_MAX_VOLUME = 15;

word audimy_default_channel = 0;   /* Default instrument == Piano */

/* Contains data structures to support ping-pong buffers and attributes */
audimy_pp_ctl_type audimy_pp_ctl;

/* Contains file data that has been stripped of <CRLF> pairs and trailing
 * spaces. This buffer is used provide tokens to the parser
 */
uint8  audimy_tok_buf[AUDIMY_MAX_LINE_LENGTH];

/* Index to the location of the next token        */
uint32 audimy_tb_start;

/* Length of current contents of the token buffer */
uint32 audimy_tb_length;

/* Points to the location where the ping-pong buffers will scanned from */
uint32 audimy_scan_ahead_index;

/* Duration of the note being played currently */
uint32 audimy_note_length = 0;

/* Num. ms. elapsed since the start of the current note */
uint32 audimy_elapsed_time = 0;

/*Difference between the note length and the actual duration for it was played*/
int32 audimy_excess_elapsed_time = 0;

/* Indicates current state of the IMY format */
typedef enum{
  AUDIMY_STATE_NONE       = 0x0,
  AUDIMY_STATE_PARSE_HEAD = 0x01,
  AUDIMY_STATE_CALC_TIME  = 0x02,
  AUDIMY_STATE_PLAYING    = 0x04
} audimy_curr_state_enum_type;

audimy_curr_state_enum_type audimy_curr_state = AUDIMY_STATE_NONE;

/* Time in ms for a loop */
uint32 audimy_loop_time = 0;

/* TRUE if need to calculate time */
boolean audimy_note_is_on = FALSE;

/* Duration of the smallest supported note (1/32) */
uint32 audimy_min_note_size;

/* No. of times to repeat the loop. 0 => forever */
byte   audimy_repeat_count;

/* Start of a loop found */
boolean audimy_loop_found;

/* No. of times the loop was repeated */
uint32 audimy_repeat_index;

/* File location where the loop started */
uint32 audimy_loop_start_index;

/* TRUE if end of a loop is found */
boolean audimy_loop_end_found;

/* File location where the loop ends */
uint32 audimy_loop_end_index;

/* Count of tokens in loop to detect empty loops */
int32  audimy_num_toks_in_loop;

/* TRUE if any paranthesised loop was found in the file */
boolean audimy_loops_present_in_file = FALSE;

/* The no. of buffers being flushed */
byte   audimy_flush_count;

/* Volume of the notes that are currently playing */
byte   audimy_current_volume = 7;

/* Circular queue of iMelody commands. These are translated from iMelody
 * to MIDI and other forms which the Sound Services supports
 */
static uint16 audimy_cmd_buf[AUDIMY_MAX_CMD_BUF_SIZE];

/* Length of the current contents of the command buffer */
byte   audimy_cmd_buf_length = 0;

/* Indexes to the start of the next command */
byte   audimy_cmd_index = 0;

boolean audimy_done_playing = TRUE; /* TRUE = done playing */
boolean audimy_done_parsing = TRUE; /* TRUE = done parsing */

/* State of the ping-pong buffers */
audimy_data_state_type audimy_data_state;

/* audimy_info contains all relevant information about the currently playing
 * iMelody file, extracted from the header.
 */
audimy_info_type audimy_info;

/* CLASS1.00 header string. The "\0" character is the placeholder for checking
 * the actual minor versions supported during header parsing. */
const char audimy_header_str[] =
  "BEGIN:IMELODY\r\nVERSION:1.\0\r\nFORMAT:CLASS1.0\r\n";

#ifdef AUDIMY_DEBUG
#error code not present
#endif

/* <EJECT> */
/*---------------------------------------------------------------------------
** AudIMY Internal Function definitions
**---------------------------------------------------------------------------
*/
/*===========================================================================

FUNCTION audimy_strncasecmp

DESCRIPTION
  This function compares two strings of size n and determines if they are
  equal. The comparison is not case sensitive.

DEPENDENCIES
  None.

RETURN VALUE
  0 if equal; Non-zero otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL byte audimy_strncasecmp(
  uint8  *s1,
  uint8  *s2,
  byte    n
)
{
  byte i = 0;

  while(i<n) {
    if(toupper(*s1++) != toupper(*s2++)) {
      return 1;
    }
    i++;
  }
  return 0;
}

/* <EJECT> */
/*===========================================================================

FUNCTION audimy_atoi

DESCRIPTION
  Attempts to extract a number from the input string.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the number is in the range specified by (min and max) (inclusive).
  FALSE otherwise.

SIDE EFFECTS
  Set the pointer variable *number to the value of the number found.

===========================================================================*/
LOCAL boolean audimy_atoi(
  uint8        *input,
  const uint32 input_size,
  uint16       *number,
  uint16       min,
  uint16       max
)
{
  uint8 buf[6];
  long  num;
  boolean status = FALSE;

  /* There is no numeric value in iMelody that gets as big as 4 digits */
  if(input_size <= 5) {

  /* Copy the input and guarantee that it is terminated by a NULL char. */
  (void)memcpy(buf, input, input_size);
  buf[input_size] = '\0';

  num = strtol((const char*)buf, NULL, 10); /* base Ten */

  if(num >= min && num <= max) {
    *number = (uint16)num;
      status = TRUE;
    }
    else {
  MSG_ERROR("Invalid number '%s'", buf,0,0);
    }
  }
  else {
    MSG_ERROR("Input string size %d too large", input_size, 0, 0);
  }

  return status;
}

/* <EJECT> */
/*===========================================================================

FUNCTION audimy_update_data_index

DESCRIPTION
  Updates token buffer's start index and length. This function must be called
  after a token is used up.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Left-shifts the remaining data, if we approaching the end of the buffer,
  so as to not step outside the buffer.

===========================================================================*/
void audimy_update_data_index(
  uint16 num_to_add
)
{
  audimy_tb_start  += num_to_add;
  audimy_tb_length -= num_to_add;

  if((audimy_tb_start + audimy_tb_length + AUDIMY_MAX_TOKEN_SIZE) >=
      AUDIMY_MAX_LINE_LENGTH) {
    /* Ensure that we don't overflow audimy_tok_buf */
    (void)memmove(audimy_tok_buf, &audimy_tok_buf[audimy_tb_start],
                  audimy_tb_length);
    audimy_tb_start = 0;
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION audimy_get_data

DESCRIPTION
  Wrapper function used to used to request data from the client. Do not use
  this function directly. Use audimy_sync_get_data() and
  audimy_async_get_data() instead.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if data requested was accepted.
  FALSE otherwise.

SIDE EFFECTS
  Sets audimy_data_state if the request fails.

===========================================================================*/
LOCAL boolean audimy_get_data(
  uint32          start_pos,
  audimy_buf_type *buffer
)
{

  if( buffer->loading){
    return TRUE;
  }
  if(audimy_data_state != AUDIMY_DATA_ERROR && !audimy_flush_count) {

    buffer->loading = TRUE;
    buffer->start = start_pos;
    if(audmain_get_data(audimy_pp_ctl.handle, buffer->start,
                        &buffer->data) != AUDMAIN_STATUS_SUCCESS) {
      MSG_ERROR("Read failed", 0,0,0);
      audimy_data_state = AUDIMY_DATA_ERROR;
      audimy_pp_ctl.error_status = AUDMAIN_STATUS_ERROR;
      return FALSE;
    }
  }
  else {
    MSG_ERROR("Invalid read request", 0,0,0);
    audimy_data_state = AUDIMY_DATA_ERROR;
    audimy_pp_ctl.error_status = AUDMAIN_STATUS_ERROR;
    return FALSE;
  }
  return TRUE;
}

/* <EJECT> */
/*===========================================================================

FUNCTION audimy_sync_get_data

DESCRIPTION
  Use this function to get data in a synchronous fashion. This is useful while
  parsing the header information.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if successful.

SIDE EFFECTS
  Updates the curr_buf pointer in audimy_pp_ctl to point to the current
  buffer.

===========================================================================*/
LOCAL boolean audimy_sync_get_data(
  uint32 current_pos
)
{
  audimy_buf_type *curr_buf = audimy_pp_ctl.curr_buf;

  /* In synchronous mode, if the current_pos has not gone beyond the end
   * of the current buffer, we still have data, and don't have to do anything.
   * Otherwise, request for more data.
   */
  if(current_pos >= audimy_pp_ctl.file_location) {

    if(audimy_pp_ctl.file_location < audimy_pp_ctl.file_len) {

      if(curr_buf->data != NULL  &&  curr_buf->length > 0) {
        /* Done with this buffer. Free it up */
        audmain_do_play_cb(audimy_pp_ctl.handle, SND_CMX_AF_FREE_BUF,
                           0, (void *)curr_buf->data);
        curr_buf->data = NULL;
      }

      if(!audimy_get_data(audimy_pp_ctl.file_location, curr_buf)) {
        return FALSE;
      }

      if(audimy_data_state != AUDIMY_DATA_READY) {
        MSG_ERROR("Unable to get data", 0,0,0);
        return FALSE;
      }
    }

    if(curr_buf->length == 0) {
      MSG_ERROR("Unexpected end of data", 0,0,0);
      return FALSE;
    }
  }
  return TRUE;
}

/* <EJECT> */
/*===========================================================================

FUNCTION audimy_async_get_data

DESCRIPTION
  Make an asynchronous data request to the client.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the requested number of bytes are available immediately.
  FALSE otherwise. Note that audimy_data_state must be checked to see if the
        read failed, or is merely pending.

SIDE EFFECTS
  Sets **buffer to point to the data buffer, if data is available.
  Updates audimy_data_state.

===========================================================================*/
LOCAL boolean audimy_async_get_data(
  uint32 relative_index, /* Start position relative to the previous position */
  uint32 num_bytes_reqd, /* Number of bytes required                         */
  uint8  **buffer        /* Pointer to the buffer                            */
)
{
  uint8           *data_p;
  int16           buf_index;
  audimy_buf_type *curr_buf       = audimy_pp_ctl.curr_buf;
  audimy_buf_type *prev_buf       = NULL;
  byte            num_buffers     = 2;
  boolean         done            = FALSE;
  boolean         audimy_lf_found = FALSE;

  if(num_bytes_reqd > AUDIMY_MAX_TOKEN_SIZE) {
    MSG_ERROR("Size of data reqd. too large %d", num_bytes_reqd,0,0);
    audimy_data_state = AUDIMY_DATA_ERROR;
    audimy_pp_ctl.error_status = AUDMAIN_STATUS_ERROR;
    return FALSE;
  }

  if(audimy_data_state == AUDIMY_DATA_ERROR) {
    /* Attempt to read past the EOF */
    audimy_data_state = AUDIMY_DATA_ERROR;
    return FALSE;
  }

  if(audimy_data_state != AUDIMY_DATA_READY) {
    return FALSE;
  }

  if(curr_buf->loading == TRUE) {
    /* Will never come here if the code is correct */
    MSG_ERROR("Invalid internal state", 0,0,0);
    audimy_data_state = AUDIMY_DATA_ERROR;
    audimy_pp_ctl.error_status = AUDMAIN_STATUS_ERROR;
    return FALSE;
  }

  if(audimy_scan_ahead_index >= audimy_pp_ctl.file_len) {
    if (relative_index+num_bytes_reqd <= audimy_tb_length) {
      *buffer = &audimy_tok_buf[audimy_tb_start+relative_index];
      return TRUE;
    }
    else {
      MSG_ERROR("Requested data beyond length of file", 0,0,0);
      audimy_data_state = AUDIMY_DATA_ERROR;
      audimy_pp_ctl.error_status = AUDMAIN_STATUS_ERROR;
      return FALSE;
    }
  }

  /* Parse a token, as large as the caller's requested size, from the current
   * current ping pong buffer.
   */
  while(num_buffers) {
    buf_index = audimy_scan_ahead_index - curr_buf->start;
    if(buf_index < 0) {
      MSG_ERROR("Buffer error", 0,0,0);
      audimy_data_state = AUDIMY_DATA_ERROR;
      audimy_pp_ctl.error_status = AUDMAIN_STATUS_ERROR;
      return FALSE;
    }

    data_p = curr_buf->data;

    while(buf_index < curr_buf->length) {

      /* Allow up to one leading space character */
      if(data_p[buf_index] == ' ') {
        MSG_LOW("Ignoring a space in the content.", 0,0,0);
      }
      /* Handle the linefeed character */
      else if(data_p[buf_index] == '\n') {
        /* Remove the ending carriage return character if it exists in the
        ** token buffer because it is not a part of the actual token
        */
        if(audimy_tb_length >= 1) {
          if(audimy_tok_buf[audimy_tb_start+audimy_tb_length-1] !=
             AUDIMY_CARRIAGE_RETURN) {

            MSG_HIGH("Ignoring unexpected linefeed char.", 0,0,0);
          }
          else {
            audimy_tb_length--;
          }
        }
        else {
          MSG_HIGH("Ignoring bad syntax: LF not preceded by a CR", 0,0,0);
        }
        audimy_lf_found = TRUE;
      }
      /* For all other characters... */
      else {
        /* Try returning the cached contents of the token buffer if the size
        ** of the read request can be satisfied now
        */
        if(relative_index+num_bytes_reqd <= audimy_tb_length) {
          /* The token buffer returned to the client must not contain any
          ** carriage return or linefeed characters
          */
          if(audimy_tok_buf[audimy_tb_start+relative_index+num_bytes_reqd-1] !=
             AUDIMY_CARRIAGE_RETURN) {
            /* DONE */
            done = TRUE;
            break;
          }

          /* Skip characters until a linefeed character is found is not allowed
          */
          MSG_ERROR("Unexpected carriage return.", 0,0,0);
          audimy_data_state = AUDIMY_DATA_ERROR;
          audimy_pp_ctl.error_status = AUDMAIN_STATUS_ERROR;
          return FALSE;
        }
        /* Stuff a new character to the end of the token buffer */
        else {
          if(data_p[buf_index] == '(' && audimy_loop_found == FALSE) {
            /* Skip the '(' char., the next time we repeat this loop. */
            audimy_loop_start_index = audimy_scan_ahead_index + 1;
          }
          else if(data_p[buf_index] == ')' && audimy_loop_end_found == FALSE) {
            audimy_loop_end_index = audimy_scan_ahead_index + 1;
          }

          audimy_tok_buf[audimy_tb_start+audimy_tb_length] = data_p[buf_index];
          audimy_tb_length++;
          audimy_lf_found = FALSE;
        }
      }
      audimy_scan_ahead_index++;
      buf_index++;
    }

    /* Always try keeping the ping pong buffers full to minimize the read
     * latency. When the first ping pong buffer is used up, switch the current
     * buffer to point to the second ping pong buffer, and vice versa.
     */
    if(buf_index >= curr_buf->length && curr_buf->loading == FALSE) {
      /* Retire the current ping pong buffer's data because it is no longer
       * needed
       */
      audmain_do_play_cb(audimy_pp_ctl.handle, SND_CMX_AF_FREE_BUF, 0,
                         (void *)curr_buf->data);
      curr_buf->data = NULL;

      /* Swap in the new current ping pong buffer */
      prev_buf = curr_buf;
      if(curr_buf == &audimy_pp_ctl.buf1) {
        curr_buf = &audimy_pp_ctl.buf2;
      }
      else {
        curr_buf = &audimy_pp_ctl.buf1;
      }
      audimy_pp_ctl.curr_buf = curr_buf;

      /* Attempt buffering only if the end of the file has not been reached */
      if(audimy_pp_ctl.file_location < audimy_pp_ctl.file_len) {
        /* Ensure that the new current ping pong buffer represents the next
         * contiguous file location if it is marked as ready for use
         */
        if(curr_buf->loading == FALSE &&
           curr_buf->start != audimy_scan_ahead_index) {

          MSG_ERROR("Buffer error", 0,0,0);
          audimy_data_state = AUDIMY_DATA_ERROR;
          audimy_pp_ctl.error_status = AUDMAIN_STATUS_ERROR;
          return FALSE;
        }

        /* Since the new current ping pong buffer appears to be okay, we will
         * try to buffer up the next file location to the previous ping pong
         * buffer
         */
        if (!audimy_get_data(audimy_pp_ctl.file_location, prev_buf)) {
          return FALSE;
        }
      }

      /* Abort parsing if the current ping pong buffer is still not marked
       * as ready for use: Parsing will resume when the data becomes available
       */
      if(curr_buf->loading == TRUE) {
        audimy_data_state = AUDIMY_DATA_LOADING;
        break;
      }
    }

    /* Return the currently parsed token to the caller if it is ready.
     * Otherwise, continue the parsing process by parsing through the new
     * current ping pong buffer. When both the first and second buffers have
     * been tried without producing a token, this routine will abort and
     * parsing will resume later when data becomes available.
     */
    if(done || (relative_index+num_bytes_reqd <= audimy_tb_length) &&
       (audimy_tok_buf[audimy_tb_start+relative_index+num_bytes_reqd-1] !=
        AUDIMY_CARRIAGE_RETURN) ) {
      *buffer = &audimy_tok_buf[audimy_tb_start+relative_index];
      return TRUE;
    }

    num_buffers--;
  }

  return FALSE;
}

/* <EJECT> */
/*===========================================================================

FUNCTION audimy_setup_to_seek

DESCRIPTION
  Flushes the current buffers, and causes data to be reloaded from the
  indicated restart_position.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void audimy_setup_to_seek(
  uint32 restart_pos
)
{
  audimy_buf_type *curr_buf = audimy_pp_ctl.curr_buf;
  audimy_flush_count = 0;
  audimy_tb_start    = 0;
  audimy_tb_length   = 0;
  audimy_scan_ahead_index = restart_pos;

  if(curr_buf && curr_buf->data != NULL) {
    /* Check if we already have the block to be repeated */
    if(curr_buf->start <= restart_pos &&
       restart_pos <= (curr_buf->start+curr_buf->length)) {
      return;
    }
  }

  /* Flush any data we have, and setup to wait for pending loads
   * to complete and flush them too, before we restart scanning.
   */
  curr_buf = &audimy_pp_ctl.buf1;
  if(curr_buf->loading == TRUE) {
    audimy_flush_count = 1;
  }
  else {
    if(curr_buf->data != NULL) {
      audmain_do_play_cb(audimy_pp_ctl.handle, SND_CMX_AF_FREE_BUF,
                         0, (void *)curr_buf->data);
      curr_buf->data = NULL;
      curr_buf->start = 0;
      curr_buf->length = 0;
    }
  }

  curr_buf = &audimy_pp_ctl.buf2;
  if(curr_buf->loading == TRUE) {
    audimy_flush_count++;
  }
  else {
    if(curr_buf->data != NULL) {
      audmain_do_play_cb(audimy_pp_ctl.handle, SND_CMX_AF_FREE_BUF,
                         0, (void *)curr_buf->data);
      curr_buf->data   = NULL;
      curr_buf->start  = 0;
      curr_buf->length = 0;
    }
  }

  audimy_pp_ctl.file_location = audimy_scan_ahead_index;
  if(audimy_flush_count) {
    audimy_data_state = AUDIMY_DATA_FLUSH;
  }
  else {
    audimy_pp_ctl.curr_buf = &audimy_pp_ctl.buf1;
    (void)audimy_get_data(audimy_pp_ctl.file_location, &audimy_pp_ctl.buf1);
    audimy_data_state = AUDIMY_DATA_LOADING;
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION audimy_get_lhs

DESCRIPTION
  This function synchronously extracts an LHS token from the input.
  Used to extract header info.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if successful.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL boolean audimy_get_lhs(
  byte   *token_size, /* Size of the token returned          */
  uint8  **output_buf /* Pointer to the data buffer returned */
) {
  static const uint16 max_lhs_size = sizeof("COMPOSER:");
  uint32 buf_index = 0;
  uint8  bi = 0;
  uint32  current_pos = audimy_scan_ahead_index;
  audimy_buf_type *curr_buf = audimy_pp_ctl.curr_buf;

  while(1) {
    /* Ensure that we have data */
    if(audimy_sync_get_data(current_pos) == FALSE) {
      return FALSE;
    }

    curr_buf = audimy_pp_ctl.curr_buf;

    buf_index = audimy_scan_ahead_index - curr_buf->start;
    while(buf_index < curr_buf->length) {
      if(bi >= max_lhs_size) {
        MSG_ERROR("LHS too long: Expecting a ':'", 0,0,0);
        return FALSE;
      }

      audimy_tok_buf[bi] = curr_buf->data[buf_index];
      buf_index++;
      if(audimy_tok_buf[bi] == ':') {
        *token_size = bi + 1;
        *output_buf = audimy_tok_buf;
        return TRUE;
      }
      bi++;
    }
    current_pos = audimy_pp_ctl.file_location;
  }
  return FALSE;
}

/* <EJECT> */
/*===========================================================================

FUNCTION audimy_get_rhs

DESCRIPTION
  This function synchronously extracts an RHS token from the input.
  Used to extract header info.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if successful.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL boolean audimy_get_rhs(
  const byte lhs_token_size,  /* Size of the preceding LHS token      */
  byte       *rhs_token_size, /* Size of the RHS returned             */
  uint8      **output_buf     /* Pointer to the buffer containing RHS */
)
{
  uint32  buf_index = 0;
  uint8   bi = 0;
  uint8   max_rhs_size = AUDIMY_MAX_LINE_LENGTH - lhs_token_size;
  uint32  current_pos = audimy_scan_ahead_index;
  audimy_buf_type *curr_buf = &audimy_pp_ctl.buf1;

  while(1) {
    /* Ensure that we have data */
    if(audimy_sync_get_data(current_pos) == FALSE) {
      return FALSE;
    }

    buf_index = audimy_scan_ahead_index - curr_buf->start;
    while(buf_index < curr_buf->length) {
      if(bi >= max_rhs_size) {
        MSG_ERROR("RHS too long: Expecting a '\n'", 0,0,0);
        return FALSE;
      }
      audimy_tok_buf[bi] = curr_buf->data[buf_index];
      buf_index++;
      if(audimy_tok_buf[bi] == '\n') {
        /* Overwrite the CRLF at the end with a NULL char. This will
         * allow the client to determine its length when we pass it up
         * to the client.
         */
        if ((bi > 0) && (audimy_tok_buf[bi-1] == AUDIMY_CARRIAGE_RETURN)) {
          audimy_tok_buf[bi-1] = '\0';
        }
        else {
          audimy_tok_buf[bi] = '\0';
        }
        *rhs_token_size = bi + 1;
        *output_buf = audimy_tok_buf;
        return TRUE;
      }
      bi++;
    }
    current_pos = audimy_pp_ctl.file_location;
  }
  return FALSE;
}

/* <EJECT> */
/*===========================================================================

FUNCTION audimy_extract_header

DESCRIPTION
  This function extracts iMelody header info.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if successful.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL boolean audimy_extract_header()
{
  uint8      *buffer;
  byte       lhs_token_size;
  byte       rhs_token_size;
  boolean    status;
  uint16     number;

  while(audimy_get_lhs(&lhs_token_size, &buffer) == TRUE) {
    audimy_scan_ahead_index += lhs_token_size;
    status = FALSE;

    switch(lhs_token_size) {
    case 4:
      if(audimy_strncasecmp(buffer, (uint8*)"END:", lhs_token_size) == 0) {
        MSG_ERROR("Unexpected use of END", 0,0,0);
      }
      break;

    case 5:
      if(audimy_strncasecmp(buffer, (uint8*)"NAME:", lhs_token_size) == 0) {
        if(audimy_get_rhs(lhs_token_size, &rhs_token_size, &buffer) == TRUE) {
          (void*)memcpy(audimy_info.name, buffer, rhs_token_size);
          status = TRUE;
        }
      }
      else if(audimy_strncasecmp(buffer, (uint8*)"BEAT:", lhs_token_size) == 0) {
        if(audimy_get_rhs(lhs_token_size, &rhs_token_size, &buffer) == TRUE) {
          if(audimy_atoi(buffer, rhs_token_size, &number, 25, 900) == TRUE) {
            audimy_info.beat = number;
            status = TRUE;
          }
          else {
            MSG_ERROR("Invalid BEAT value", 0,0,0);
          }
        }
      }
      break;

    case 6:
      if(audimy_strncasecmp(buffer, (uint8*)"STYLE:", lhs_token_size) == 0) {
        if(audimy_get_rhs(lhs_token_size, &rhs_token_size, &buffer) == TRUE) {
          /* Skip optional leading S */
          if(buffer[0] == 'S') {
            buffer++;
            rhs_token_size--;
            audimy_scan_ahead_index++;
          }

          if(audimy_atoi(buffer, rhs_token_size, &number, 0, 2) == TRUE) {
            audimy_info.style = number;
            status = TRUE;
          }
          else {
            MSG_ERROR("Invalid STYLE value", 0,0,0);
          }
        }
      }
      break;

    case 7:
      if(audimy_strncasecmp(buffer, (uint8*)"VOLUME:", lhs_token_size) == 0) {
        if(audimy_get_rhs(lhs_token_size, &rhs_token_size, &buffer) == TRUE) {
          if(buffer[0] == 'V') {
            buffer++;
            rhs_token_size--;
            audimy_scan_ahead_index++;
          }

          if(buffer[0] == '-' || buffer[0] == '+') {
            audimy_info.volume = 6;
            status = TRUE;
          }
          else if(buffer[0] == '+') {
            audimy_info.volume = 8;
            status = TRUE;
          }
          else {
            if(audimy_atoi(buffer, rhs_token_size, &number, 0,
                           AUDIMY_MAX_VOLUME) == TRUE) {
              audimy_info.volume = (byte)number;
              status = TRUE;
            }
            else {
              MSG_ERROR("Volume level out of range", 0,0,0);
            }
          }
        }
      }
      if(audimy_strncasecmp(buffer, (uint8*)"MELODY:", lhs_token_size) == 0) {
        /* Done scanning header info */
        return TRUE;
      }
      break;

    case 9:
      if(audimy_strncasecmp(buffer, (uint8*)"COMPOSER:", lhs_token_size) == 0) {
        if(audimy_get_rhs(lhs_token_size, &rhs_token_size, &buffer) == TRUE) {
          (void*)memcpy(audimy_info.composer, buffer, rhs_token_size);
          status = TRUE;
        }
      }
      break;

    default:
      break;
    } /* switch(lhs_token_size) */

    if(status == FALSE) {
      break;
    }

    audimy_scan_ahead_index += rhs_token_size;
  } /* while() */

  MSG_ERROR("Unexpected token", 0,0,0);
  return FALSE;
}

/* <EJECT> */
/*===========================================================================

FUNCTION audimy_do_file_spec

DESCRIPTION
  This function builds a sound file specifications buffer and transmits the
  data to the CMX layer through the playback callback.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void audimy_do_file_spec (
  snd_af_access_type          *handle
)
{
  snd_imy_spec_type snd_spec;

  snd_spec.file_type = SND_CMX_AF_FILE_IMY;
  snd_spec.format    = audimy_info.format;
  snd_spec.beat      = audimy_info.beat;
  snd_spec.style     = audimy_info.style;
  snd_spec.supported_ops = SND_SUPPORTED_OPS_MASK_GETTIME;

  memcpy(snd_spec.name, audimy_info.name, sizeof(snd_spec.name));
  memcpy(snd_spec.composer, audimy_info.composer, sizeof(snd_spec.composer));

  audmain_do_play_cb( handle, SND_CMX_AF_AUDIO_SPEC, 0, (void *)&snd_spec );
}

/* <EJECT> */
/*===========================================================================

FUNCTION audimy_add_mel_cmd

DESCRIPTION
  Enqueues an iMelody command to the circular queue audimy_cmd_buf.

DEPENDENCIES
  Assumes that the caller has previously ensured that there is sufficient
  space in the queue for the new command.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void audimy_add_mel_cmd(
  audimy_mel_type type,
  word            *buffer
)
{
  byte num_copied;
  byte start;
  byte length;

  if(type >= AUDIMY_MEL_MAX) {
    MSG_ERROR("Invalid IMY cmd %d", (int)type, 0,0);
    audimy_done_playing = TRUE;
    return;
  }

  length = audimy_cmd_len[type];

  if(audimy_cmd_buf_length+length+1 > AUDIMY_MAX_CMD_BUF_SIZE) {
    MSG_ERROR("Buffer overflow", 0,0,0);
    audimy_done_playing = TRUE;
    return;
  }

  start = (audimy_cmd_index+audimy_cmd_buf_length) % AUDIMY_MAX_CMD_BUF_SIZE;
  audimy_cmd_buf[start] = type;
  start++;

  if(start >= AUDIMY_MAX_CMD_BUF_SIZE) {
    start = 0;
  }

  if(start+length <= AUDIMY_MAX_CMD_BUF_SIZE) {
    (void)memcpy(&audimy_cmd_buf[start], buffer, length*2);
  }
  else {
    num_copied = AUDIMY_MAX_CMD_BUF_SIZE - start;
    (void)memcpy(&audimy_cmd_buf[start], buffer, num_copied*2);
    (void)memcpy(audimy_cmd_buf, &buffer[num_copied], (length-num_copied)*2);
  }

  audimy_cmd_buf_length += length + 1;
}

/* <EJECT> */
/*===========================================================================

FUNCTION audimy_get_mel_cmd

DESCRIPTION
  Get an iMelody command from the circular queue audimy_cmd_buf.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if a command is available.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL boolean audimy_get_mel_cmd(
  word **buffer /* Pointer to the command to be returned */
)
{
  static  word cmd_buf[5];
  boolean status = FALSE;
  byte    length = 0;
  byte    num_copied;

  if(audimy_cmd_buf_length <= 1) {
    audimy_done_playing = TRUE;
    return status;
  }

  if(audimy_cmd_buf[audimy_cmd_index] >= AUDIMY_MEL_MAX) {
    MSG_ERROR("Unknown IMY command %d", audimy_cmd_buf[audimy_cmd_index],0,0);
    audimy_done_playing = TRUE;
    return FALSE;
  }

  length = audimy_cmd_len[ audimy_cmd_buf[audimy_cmd_index] ] + 1;

  if(length) {
    if(length <= audimy_cmd_buf_length) {
      if(audimy_cmd_index+length <= AUDIMY_MAX_CMD_BUF_SIZE) {
        *buffer = &audimy_cmd_buf[audimy_cmd_index];
        audimy_cmd_index += length;
        if(audimy_cmd_index == AUDIMY_MAX_CMD_BUF_SIZE) {
          audimy_cmd_index = 0;
        }
      }
      else {
        num_copied = AUDIMY_MAX_CMD_BUF_SIZE - audimy_cmd_index;
        (void)memcpy(cmd_buf, &audimy_cmd_buf[audimy_cmd_index], num_copied*2);
        (void)memcpy(&cmd_buf[num_copied], audimy_cmd_buf, (length-num_copied)*2);

        audimy_cmd_index = length - num_copied;
        *buffer = cmd_buf;
      }
      status = TRUE;
    }
    else {
      MSG_ERROR("Buffer underflow", 0,0,0);
      audimy_done_playing = TRUE;
      return status;
    }
  }
  else {
    status = TRUE;
  }

  audimy_cmd_buf_length -= length;
  return status;
}

/* <EJECT> */
/*===========================================================================

FUNCTION audimy_parse_toggle_switch

DESCRIPTION
  This function parses toggle switches such as 'ledon/ledoff', 'vibeon/vibeoff'.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if a valid switch was found.
  If a FALSE value is returned, and audimy_data_state == AUDIMY_DATA_ERROR,
  then there was an error. Otherwise, the failure was bcoz of a pending
  read operation.

SIDE EFFECTS
  Updates the token indices if a switch was found.

===========================================================================*/
LOCAL boolean audimy_parse_toggle_switch(
  char    *switch_name, /* INPUT: switch name                */
  byte    length,       /* INPUT: length of the input string */
  boolean *enabled      /* OUTPUT: TRUE if switch is ON      */
) {
  uint8*  buffer;
  uint32  index  = 0;
  boolean status = FALSE;

  /* If this is valid file, there will at least 11 characters remaining */
  if(audimy_async_get_data(0, length+2, &buffer)) {
    if(strncmp(switch_name, (char*)buffer, length) == 0) {
      index += length;

      if(buffer[index] == 'n') {
        *enabled = TRUE;
        audimy_update_data_index(length+1);
        status = TRUE;
      }
      else if(strncmp("ff", (char*)&buffer[index], 2) == 0) {
        audimy_update_data_index(length+2);
        *enabled = FALSE;
        status = TRUE;
      }
      else {
        MSG_ERROR("Invalid switch name", 0,0,0);
      }
    }
    else {
      MSG_ERROR("Invalid switch name", 0,0,0);
    }
  }

  return status;
}

/* <EJECT> */
/*===========================================================================

FUNCTION audimy_parse_backlight

DESCRIPTION
  This function parses the backlight switch.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  TRUE if a valid switch was found.
  If a FALSE value is returned, and audimy_data_state == AUDIMY_DATA_ERROR,
  then there was an error. Otherwise, the failure was bcoz of a pending
  read operation.

SIDE EFFECTS
  Updates the token indices _only_ if a switch was found.

===========================================================================*/
LOCAL boolean audimy_parse_backlight(
  boolean *is_note /* OUTPUT: TRUE if input string was a 'b' note */
)
{
  word   tmp_cmd_buf;
  uint8  *buffer;
  boolean status = FALSE;
  *is_note = FALSE;

  /* If this is valid file, there will at least 11 characters remaining */
  if(audimy_async_get_data(0, 7, &buffer)) {
    if(strncmp("backo", (char*)buffer, 5) == 0) {

      if(buffer[5] == 'n') {
        /* Turn on backlight */
        audimy_pp_ctl.backlight_en = TRUE;
        audimy_update_data_index(6);
        tmp_cmd_buf = TRUE;
        audimy_add_mel_cmd(AUDIMY_MEL_BACKLIGHT, &tmp_cmd_buf);
        status = TRUE;
      }
      else {
        if(strncmp("ff", (char*)&buffer[5], 2) == 0) {
          /* Turn off backlight */
          audimy_pp_ctl.backlight_en = FALSE;
          audimy_update_data_index(7);
          tmp_cmd_buf = FALSE;
          audimy_add_mel_cmd(AUDIMY_MEL_BACKLIGHT, &tmp_cmd_buf);
          status = TRUE;
        }
      }
    }
    else {
      *is_note = TRUE;
      status = TRUE;
    }
  }

  return status;
}

/* <EJECT> */
/*===========================================================================

FUNCTION audimy_parse_volume

DESCRIPTION
  Extracts the volume command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL boolean audimy_parse_volume()
{
  uint16  number;
  uint8   *buffer;
  uint16  cmd_buf[5];

  if(audimy_async_get_data(1, 2, &buffer)) {
    switch(buffer[0]) {
    case '+':
      /* Increase volume if possible */
      audimy_update_data_index(2);
      if(audimy_info.volume < AUDIMY_MAX_VOLUME) {
        audimy_info.volume++;
      }
      else {
        MSG_HIGH("Ignoring volume increase", 0,0,0);
      }
      break;

    case '-':
      /* Decrease volume if possible */
      audimy_update_data_index(2);
      if(audimy_info.volume > 0) {
        audimy_info.volume--;
      }
      else {
        MSG_HIGH("Ignoring volume decrease", 0,0,0);
      }
      break;

    default:
      if(audimy_atoi(buffer, 2, &number, 0, AUDIMY_MAX_VOLUME) == TRUE) {
        audimy_update_data_index( (number < 10)? 2: 3 );
        audimy_info.volume = number;
      }
      else {
        MSG_ERROR("Invalid Volume level %d", number, 0,0);
        return FALSE;
      }
      break;
    }
    number = AUDIMY_CALC_VOLUME(audimy_info.volume);

    cmd_buf[0] = AUDIMY_MIDI_CONTROL_CHANGE;
    cmd_buf[1] = audimy_default_channel;
    cmd_buf[2] = 0x7;
    cmd_buf[3] = number;
    audimy_add_mel_cmd(AUDIMY_MEL_VOLUME, cmd_buf);
    return TRUE;
  }
  return FALSE;
}

/* <EJECT> */
/*===========================================================================

FUNCTION audimy_parse_duration

DESCRIPTION
  Extracts the duration of a note/rest.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL boolean audimy_parse_duration(
  uint32 data_index,
  uint8  *num_chars_used,
  uint16 *duration_p
)
{
  uint16  number;
  uint8   *buffer;
  boolean status = FALSE;
  uint16  duration;

  *num_chars_used = 1;

  if(audimy_async_get_data(data_index, 2, &buffer)) {
    if(audimy_atoi(buffer, 1, &number, 0, 5)) {

      /* Set duration */
      duration = audimy_min_note_size << (5 - number);

      switch(buffer[1]) {
      case '.':
        /* (1 + 1/2) Duration */
        duration += (duration >> 1);
        (*num_chars_used)++;
        break;

      case ':':
        /* (1 + 1/2 + 1/4 == 1 + 3/4) Duration */
        duration += (3 * duration) >> 2;
        (*num_chars_used)++;
        break;

      case ';':
        /* 2/3 Duration */
        duration = (duration << 1) / 3;
        (*num_chars_used)++;
        break;

      default:
        /* No <duration-specifier> present */
        break;
      }

      *duration_p = duration / 10;
      status = TRUE;
    }
    else {
      MSG_ERROR("Invalid duration '%c'", buffer[0], 0,0);
    }
  }
  return status;
}

/* <EJECT> */
/*===========================================================================

FUNCTION audimy_process_note

DESCRIPTION
  Extracts a note.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL boolean audimy_process_note()
{
  uint8   *buffer;
  uint8   half_step = '-';
  uint8   num_dur_chars;
  uint8   data_index = 0;
  uint8   data_index_start = data_index;
  uint8   note = '\0';
  uint16  number;
  byte    note_index;
  uint16  octave = 4;
  uint16  duration;
  uint16  cmd_buf[5];

  if(audimy_async_get_data(data_index, 2, &buffer)) {
    if(buffer[0] == '*') {
      if(audimy_atoi(&buffer[1], 1, &number, 0, 8)) {
        /* Set octave */
        octave = number;
        data_index += 2;

        if(audimy_async_get_data(data_index, 2, &buffer)) {
          note = buffer[0];
        }
        else {
          return FALSE;
        }
      }
      else {
        MSG_ERROR("Invalid octave specifier '%c'", buffer[0], 0,0);
        return FALSE;
      }
    }
  }
  else {
    return FALSE;
  }

  if(buffer[0] == '&' || buffer[0] == '#') {
    half_step = buffer[0];
    note = buffer[1];
    data_index++;
  }
  else {
    note = buffer[0];
  }

  switch(note) {
  case 'a':
    note_index = 0;
    break;
  case 'b':
    note_index = 2;
    break;
  case 'c':
    note_index = 3;
    break;
  case 'd':
    note_index = 5;
    break;
  case 'e':
    note_index = 7;
    break;
  case 'f':
    note_index = 8;
    break;
  case 'g':
    note_index = 10;
    break;
  default:
    MSG_ERROR("Invalid note '%c'", note, 0,0);
    return FALSE;
  }
  data_index++;

  if(half_step == '#') {
    /* A half-step up */
    note_index++;
  }
  else if(half_step == '&') {
    /* A half-step down */
    if(note == 'a') {
      note_index = 11;
    }
    else {
      note_index--;
    }
  }
  /* Determine MIDI Note Number; Down shift by one octave - otherwise the notes
   * sound too high.
   */
  note_index = 21 + (octave * 12) + note_index;

  if(audimy_parse_duration(data_index, &num_dur_chars, &duration)) {
    /* Valid note */

    audimy_update_data_index(data_index - data_index_start + num_dur_chars);

    cmd_buf[0] = duration;
    cmd_buf[1] = AUDIMY_MIDI_NOTE_ON;
    cmd_buf[2] = audimy_default_channel;
    cmd_buf[3] = note_index;
    cmd_buf[4] = 0x7f; /* Highest velocity */

    audimy_add_mel_cmd(AUDIMY_MEL_NOTE_ON, cmd_buf);

    /* Duration of Note */
    if(audimy_curr_state == AUDIMY_STATE_CALC_TIME) {
      if(audimy_loop_found) {
        audimy_loop_time += duration;
      }
      else {
        audimy_info.time_ms += duration;
      }
    }

    switch(audimy_info.style) {
    case 0: /* Natural style   (20:1) */
      duration = duration/20;
      break;

    case 1: /* Continous style (0)    */
      duration = 0;
      break;

    case 2: /* Staccato sytle  (1:1)  */
      break;

    default: /* Will never come here  */
      break;
    }
    cmd_buf[0] = duration;
    cmd_buf[1] = AUDIMY_MIDI_NOTE_OFF;
    audimy_add_mel_cmd(AUDIMY_MEL_NOTE_OFF, cmd_buf);

    /* Duration of silence after note */
    if(audimy_curr_state == AUDIMY_STATE_CALC_TIME) {
      if(audimy_loop_found) {
        audimy_loop_time += duration;
      }
      else {
        audimy_info.time_ms += duration;
      }
    }
  }
  else {
    MSG_ERROR("Bad duration value", 0,0,0);
    return FALSE;
  }

  return TRUE;
}


/* <EJECT> */
/*===========================================================================

FUNCTION audimy_parse_loop_end

DESCRIPTION
  Parses the end of a loop.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean audimy_parse_loop_end()
{
  uint8   *buffer;
  uint16  number;
  byte    tmp = 2;
  boolean is_parenthesized_loop = FALSE;

  if(audimy_loop_found == FALSE) {
    if (audimy_loops_present_in_file == TRUE) {
      /* Un-parenthesised loop only supported if there is no other
         legal loop in the file, because this is equivalent to a nested loop.
         Nested loops are not supported. */
      MSG_ERROR("Illegal loop syntax", 0, 0, 0);
      return FALSE;
    }
  }
  if(audimy_num_toks_in_loop <= 0) {
    MSG_ERROR("Empty loop", 0,0,0);
    return FALSE;
  }

  if(audimy_async_get_data(1, 9, &buffer)) {

    /* Get the number of times the block should be repeated. */
    if(!audimy_atoi(buffer, 2, &number, 0, 99)) {
      MSG_ERROR("Repeat count can't be greater that 99", 0,0,0);
      return FALSE;
    }
    if(number > 9) {
      tmp = 3;
    }

    /* Note down the repeat count only on the first pass of the loop */
    if(audimy_loop_end_found == FALSE) {
      audimy_loop_end_found = TRUE;
      audimy_repeat_count = number;
      audimy_repeat_index = 0;
    }

    audimy_update_data_index(tmp);

    if(!audimy_async_get_data(0, 1, &buffer)) {
      /* This is guaranteed to succeed; If it doesn't we have bug in
       * data buffering code */
      return FALSE;
    }

    /* Process optional <volume-modifier> */
    if(buffer[0] == 'V') {
      /* No need to check for audimy_awaiting_data after audimy_parse_volume().
       * Already have data. */
      if(!audimy_parse_volume() || !audimy_async_get_data(0, 1, &buffer)) {
        return FALSE;
      }
    }

    if(buffer[0] == ')') {
      audimy_loops_present_in_file = TRUE;
      is_parenthesized_loop = TRUE;
    }
    else {
      MSG_ERROR("Can't find a ')' to terminate loop", 0,0,0);
      /* Hack that handles non-compliant files where the loop is not
         parenthesized:
           BEGIN:IMELODY
           VERSION:1.2
           FORMAT:CLASS1.0
           MELODY:*4d3e3#f3g3a3b3*5#c3d1@3
           END:IMELODY

         We will treat this as a loop on the entire file. Such a loop will be
         allowed only if it is at the end of the file.

         Also, handle open parenthesis (unbalanced loop):
           BEGIN:IMELODY
           VERSION:1.2
           FORMAT:CLASS1.0
           MELODY:*4d3e3#f3(g3a3b3*5#c3d1@3
           END:IMELODY

         by treating the last open parenthesis '(' as the start and the end of
         the file as the ending of the loop.
      */

      if(buffer[0] == 'E') {
        if(!audimy_async_get_data(0, 3, &buffer)) {
          /* This is guaranteed to succeed; If it doesn't we have bug in
           * data buffering code */
          return FALSE;
        }
        if(audimy_strncasecmp(buffer, (uint8*)"END", 3) != 0) {
          MSG_ERROR("Unexpected '@'", 0, 0, 0);
          return FALSE;
        }
      }
      else {
        MSG_ERROR("Un-parenthesized loop", 0,0,0);
        return FALSE;
      }
    }

    if(audimy_curr_state == AUDIMY_STATE_CALC_TIME) {
      if(audimy_repeat_count == 0) {
        audimy_info.time_ms = 0xffffffff;
        audimy_loop_time = 0;
        audimy_loop_found = audimy_loop_end_found = FALSE;
        audimy_done_parsing = TRUE;
        audimy_curr_state = AUDIMY_STATE_NONE;
        return TRUE;
      }
      else {
        if(is_parenthesized_loop ||
           (audimy_loop_found && (is_parenthesized_loop == FALSE))) {

          audimy_info.time_ms += (audimy_loop_time * (audimy_repeat_count+1));
          audimy_loop_time = 0;
        }
        else {
          /* Use the entire file duration since we are looping the whole
          ** file for loops with no parenthesis present
          */
          audimy_info.time_ms *= (audimy_repeat_count+1);
        }
        audimy_repeat_index = audimy_repeat_count;
      }
    }

    audimy_num_toks_in_loop = 0;
    if (audimy_repeat_count) {
      if (audimy_repeat_index < audimy_repeat_count) {
        /* Repeat loop */
        audimy_setup_to_seek(audimy_loop_start_index);
        audimy_repeat_index++;
      }
      else {
        /* Done repeating; continue from the end of the loop */
        if (is_parenthesized_loop) {
          audimy_setup_to_seek(audimy_loop_end_index);
        }
        audimy_loop_found = FALSE;
        audimy_loop_end_found = FALSE;
        audimy_repeat_count = 0;
        audimy_repeat_index = 0;
      }
    }
    else {
      /* Is an infinite loop */
      audimy_setup_to_seek(audimy_loop_start_index);
    }
  }
  /* else waiting for data */

  return TRUE;
}


/*===========================================================================

FUNCTION audimy_parse_end

DESCRIPTION
  Parses the END marker of the song.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the data was successfully obtained

SIDE EFFECTS
  None.
===========================================================================*/
boolean audimy_parse_end(
  boolean  *is_note /* TRUE if this actually an 'e' note */
)
{
  uint8   *buffer;
  boolean status = FALSE;
  *is_note = FALSE;

  if(audimy_async_get_data(0, 11, &buffer)) {
    if(audimy_strncasecmp(buffer, (uint8*)"END:IMELODY", 11) != 0) {
      /* Is actually an 'e' note */
      *is_note = TRUE;
    }
    status = TRUE;
  }
  return status;
}

/* <EJECT> */
/*---------------------------------------------------------------------------
** AUDIMY External Function definitions
**---------------------------------------------------------------------------
*/

/*===========================================================================

FUNCTION audimy_get_data_cb_req

DESCRIPTION
  Callback function which handles the client's response to a Data request.

DEPENDENCIES
  audimy_pp_ctl

RETURN VALUE
  TRUE if the data was successfully obtained

SIDE EFFECTS
  audimy_pp_ctl

===========================================================================*/
boolean audimy_get_data_cb_req(
  uint8  **data,
  uint8  *buf,
  uint32 length,
  uint32 pos
)
{
  audimy_buf_type *buffer, *other_buf;

  *data = buf;

  if(buf == NULL) {
    MSG_ERROR("No data received", 0,0,0);
    audimy_pp_ctl.curr_buf->length = 0;
    audimy_pp_ctl.curr_buf->loading = FALSE;
    audimy_data_state = AUDIMY_DATA_ERROR;
    audimy_pp_ctl.error_status = AUDMAIN_STATUS_DATA_ACCESS_ERR;
    return TRUE;
  }

  /* Store received length for the buffer processed. */
  if(buf == audimy_pp_ctl.buf1.data) {
    buffer    = &audimy_pp_ctl.buf1;
    other_buf = &audimy_pp_ctl.buf2;
  }
  else if(buf == audimy_pp_ctl.buf2.data) {
    buffer    = &audimy_pp_ctl.buf2;
    other_buf = &audimy_pp_ctl.buf1;
  }
  else {
    MSG_ERROR("Misplaced buffer!",0,0,0);
    audimy_data_state = AUDIMY_DATA_ERROR;
    audimy_pp_ctl.error_status = AUDMAIN_STATUS_ERROR;
    return FALSE;
  }

  if(audimy_flush_count) {
    if(audimy_data_state == AUDIMY_DATA_FLUSH) {
      audmain_do_play_cb(audimy_pp_ctl.handle, SND_CMX_AF_FREE_BUF,
                         0, (void *)buffer->data);
      buffer->data = NULL;
      buffer->start = 0;
      buffer->length = 0;

      audimy_flush_count--;

      if(audimy_flush_count == 0) {
        audimy_data_state         = AUDIMY_DATA_LOADING;
        buffer                    = &audimy_pp_ctl.buf1;
        audimy_pp_ctl.curr_buf = buffer;
        if (!audimy_get_data(audimy_loop_start_index, buffer)) {
          return FALSE;
        }
      }
    }
    else {
      MSG_ERROR("Invalid state", 0,0,0);
      return FALSE;
    }
  }
  else {
    if(pos != buffer->start) {
      MSG_ERROR("Expecting data from %d, got from %d", buffer->start, pos, 0);
      audimy_data_state = AUDIMY_DATA_ERROR;
      audimy_pp_ctl.error_status = AUDMAIN_STATUS_ERROR;
      return FALSE;
    }
    audimy_pp_ctl.file_location = buffer->start + length;
    buffer->length = length;
    buffer->loading = FALSE;

    if(audimy_data_state != AUDIMY_DATA_ERROR) {
      audimy_data_state = AUDIMY_DATA_READY;

      if(audimy_pp_ctl.file_location <= audimy_pp_ctl.file_len) {
        if(other_buf->data == NULL) {
          if (!audimy_get_data(audimy_pp_ctl.file_location, other_buf)) {
            return FALSE;
          }
        }
      }
    }
  }

  return TRUE;
}

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
audmain_status_type audimy_parse_head (
  uint8                       *buffer,
  snd_af_access_type          *handle,
  audmain_parse_ctl_type      *parse_ctl
) {
  word mel_cmd_buf[5];
  uint32 header_size  = sizeof(audimy_header_str) - 1;
  uint32 header_index = 0;
  uint32 buffer_index = 0;
  boolean bad_header_flag = FALSE;

  audimy_reset();

  /* Check for supported iMelody versions */
  while(header_index < header_size) {
    if(buffer[buffer_index] != audimy_header_str[header_index]) {
      /* Skip carriage return character differences */
      if((audimy_header_str[header_index] == AUDIMY_CARRIAGE_RETURN) &&
         (buffer[buffer_index] == '\n')) {

        /* Ignore carriage returns */
      }
      /* Allow only supported version numbers; the '\0' character in the
      ** header string tells us to compare the minor version character
      */
      else if((audimy_header_str[header_index] == '\0') &&
              (buffer[buffer_index] == '0' || buffer[buffer_index] == '2')) {

        buffer_index += 1;
      }
      /* Abort due to unrecognized header character in buffer */
      else {
        bad_header_flag = TRUE;
        break;
      }
    }
    else {
      buffer_index += 1;
    }

    header_index += 1;
  }

  if (bad_header_flag) {
    MSG_MED("Not an iMelody file or unsupported version", 0, 0, 0);
    return AUDMAIN_STATUS_ERROR;
  }

  /* Initialize the iMelody control structure */
  audimy_scan_ahead_index     = buffer_index;

  audimy_pp_ctl.handle        = handle;
  audimy_pp_ctl.file_location = parse_ctl->buffer_size;

  audimy_pp_ctl.buf1.data     = buffer;
  audimy_pp_ctl.buf1.start    = 0;
  audimy_pp_ctl.buf1.length   = parse_ctl->buffer_size;
  audimy_pp_ctl.buf1.loading  = FALSE;

  audimy_pp_ctl.buf2.data     = NULL;
  audimy_pp_ctl.buf2.start    = 0;
  audimy_pp_ctl.buf2.length   = 0;
  audimy_pp_ctl.buf2.loading  = FALSE;

  audimy_pp_ctl.curr_buf      = &audimy_pp_ctl.buf1;
  audimy_pp_ctl.error_status  = AUDMAIN_STATUS_MAX;

  /* Extract iMelody header from the input stream */
  if (audimy_extract_header() != TRUE) {
    return AUDMAIN_STATUS_ERROR;
  }

  /* Get the duration (in ms) of the smallest
   * supported integral note length (1/32).
   */
  audimy_min_note_size = 75000/audimy_info.beat;

  audmain_do_play_cb(handle, SND_CMX_AF_GET_FILE_LEN, 0,
                     (void *)&audimy_pp_ctl.file_len);

  audimy_loop_start_index = audimy_scan_ahead_index;

  /* Switch to async. reads */
  audmain_set_read_sync(FALSE);

  /* Load in the second buffer */
  if(audimy_pp_ctl.file_location < audimy_pp_ctl.file_len) {
    (void)audimy_get_data(audimy_pp_ctl.file_location,
                          &audimy_pp_ctl.buf2);
  }

  /* Create the SND/CMX file spec table, and return the data to the CMX layer */
  audimy_do_file_spec(handle);

  /* Reset the Qsynth player */
  mel_cmd_buf[0] = AUDIMY_MIDI_RESET;
  audimy_add_mel_cmd(AUDIMY_MEL_RESET, mel_cmd_buf);

  /* Program change : use channel 0 */
  mel_cmd_buf[0] = AUDIMY_MIDI_PROGRAM_CHANGE;
  mel_cmd_buf[1] = audimy_default_channel;
  mel_cmd_buf[2] = 0x00;
  audimy_add_mel_cmd(AUDIMY_MEL_PROGRAM, mel_cmd_buf);

  mel_cmd_buf[0] = 200;
  mel_cmd_buf[1] = AUDIMY_MIDI_NOTE_OFF;
  mel_cmd_buf[2] = audimy_default_channel;
  mel_cmd_buf[3] = 60;
  mel_cmd_buf[4] = 0x7f; /* Highest velocity */
  audimy_add_mel_cmd(AUDIMY_MEL_NOTE_OFF, mel_cmd_buf);

  /* Set the volume to the default volume */
  mel_cmd_buf[0] = AUDIMY_MIDI_CONTROL_CHANGE;
  mel_cmd_buf[1] = audimy_default_channel;
  mel_cmd_buf[2] = 7;
  mel_cmd_buf[3] = AUDIMY_CALC_VOLUME(audimy_info.volume);
  audimy_add_mel_cmd(AUDIMY_MEL_VOLUME, mel_cmd_buf);

  MSG_MED("iMelody ready to go", 0, 0, 0);
  return(AUDMAIN_STATUS_SUCCESS);
} /* end audimy_parse_head */

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
void audimy_set_instrument (
  word instrument
)
{
  audimy_default_channel = instrument;
}

/* <EJECT> */
/*===========================================================================

FUNCTION audimy_get_sample_rate

DESCRIPTION
  This function returns the sampling rate of the current imy song.

DEPENDENCIES
  audimy_info

RETURN VALUE
  sample rate of the last imy file parsed

SIDE EFFECTS
  None

===========================================================================*/
snd_sample_rate_type audimy_get_sample_rate (
  audmain_parse_ctl_type  *parse_ctl
)
{
  return SND_SAMPLE_RATE_32000;
}

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
void audimy_reset ()
{
  (void)memset(&audimy_info, 0, sizeof(audimy_info));

  /* Set the defaults */
  audimy_info.beat   = 120;
  audimy_info.style  = 0;
  audimy_info.volume = 7;

  audimy_scan_ahead_index = 0;
  audimy_tb_start         = 0;
  audimy_tb_length        = 0;

  audimy_cmd_index           = 0;
  audimy_cmd_buf_length      = 0;
  audimy_elapsed_time        = 0;
  audimy_excess_elapsed_time = 0;
  audimy_note_length         = 0;
  audimy_loop_time           = 0;
  audimy_curr_state          = AUDIMY_STATE_NONE;

  audimy_done_playing = FALSE;
  audimy_done_parsing = FALSE;

  audimy_repeat_count          = 0;
  audimy_repeat_index          = 0;
  audimy_loop_start_index      = 0;
  audimy_loop_found            = FALSE;
  audimy_loop_end_found        = FALSE;
  audimy_num_toks_in_loop      = -1;
  audimy_loops_present_in_file = FALSE;

  audimy_flush_count = 0;

  audimy_data_state     = AUDIMY_DATA_READY;
  audimy_note_is_on     = FALSE;
  audimy_current_volume = 7;

  audimy_pp_ctl.led_en       = FALSE;
  audimy_pp_ctl.vib_en       = FALSE;
  audimy_pp_ctl.backlight_en = FALSE;
}

/* <EJECT> */
/*===========================================================================

FUNCTION audimy_parse

DESCRIPTION
  Parses the data file and generates equivalent MIDI commands.

DEPENDENCIES
  Must call audimy_age() before calling this function!

RETURN VALUE
  AUDMAIN_STATUS_DONE    - done parsing
  AUDMAIN_STATUS_SUCCESS - sucessfully parsed tokens
  AUDMAIN_STATUS_ERROR   - Error in parsed data.

SIDE EFFECTS
  Sets audimy_done_parsing to TRUE after the entire file is parsed.

===========================================================================*/
audmain_status_type audimy_parse (
  audmain_parse_ctl_type  *parse_ctl,
  audmain_cmd_buf_type    *cmd_buf,
  uint32                  *time_remain
)
{
  word    mel_cmd_buf[5];
  uint8   *buffer;
  audmain_status_type status = AUDMAIN_STATUS_ERROR;
  boolean is_note;
  uint8   num_chars_used;
  uint16  duration;


  if(audimy_done_playing == TRUE) {
    return AUDMAIN_STATUS_DONE;
  }

  *time_remain = audimy_note_length - audimy_elapsed_time;

  if(audimy_flush_count || audimy_done_parsing == TRUE) {
    return AUDMAIN_STATUS_SUCCESS;
  }

  while(audimy_cmd_buf_length+12 <= AUDIMY_MAX_CMD_BUF_SIZE) {

    if(!audimy_async_get_data(0, 1, &buffer)) {
      if(audimy_data_state == AUDIMY_DATA_ERROR) {
        return audimy_pp_ctl.error_status;
      }

      /* There must be some pending data operation; wait for it to complete */
      return AUDMAIN_STATUS_SUCCESS;
    }

    switch(buffer[0]) {
    case 'l': /* <led> */
      if(audimy_parse_toggle_switch("ledo", 4, &audimy_pp_ctl.led_en)) {
        /* Toggle LED */
        mel_cmd_buf[0] = audimy_pp_ctl.led_en;
        audimy_add_mel_cmd(AUDIMY_MEL_LED, mel_cmd_buf);
        status = AUDMAIN_STATUS_SUCCESS;
      }
      break;

    case 'v': /* <vib> */
      if(audimy_parse_toggle_switch("vibeo", 5, &audimy_pp_ctl.vib_en)) {
        /* Toggle Vibrator */
        mel_cmd_buf[0] = audimy_pp_ctl.vib_en;
        audimy_add_mel_cmd(AUDIMY_MEL_VIBRATOR, mel_cmd_buf);
        status = AUDMAIN_STATUS_SUCCESS;
      }
      break;

    case 'V': /* <volume> */
      if(audimy_parse_volume()) {
        status = AUDMAIN_STATUS_SUCCESS;
      }
      break;

    case 'r': /* <silence> */
      if(audimy_parse_duration(1, &num_chars_used, &duration)) {
        /* Rest note */
        audimy_update_data_index(num_chars_used + 1);

        switch(audimy_info.style) {
        case 0: /* Natural style   (20:1) */
          duration = duration + (duration/20);
          break;

        case 1: /* Continous style (0)    */
          /* duration = duration; */
          break;

        case 2: /* Staccato sytle  (1:1)  */
          duration += duration;
          break;

        default: /* Will never come here  */
          break;
        }
        mel_cmd_buf[0] = duration;
        mel_cmd_buf[1] = AUDIMY_MIDI_NOTE_OFF;
        mel_cmd_buf[2] = audimy_default_channel;
        mel_cmd_buf[3] = 60;
        mel_cmd_buf[4] = 0;
        audimy_add_mel_cmd(AUDIMY_MEL_NOTE_OFF, mel_cmd_buf);

        /* Duration of rest note */
        if(audimy_curr_state == AUDIMY_STATE_CALC_TIME) {
          if(audimy_loop_found) {
            audimy_loop_time += duration;
          }
          else {
            audimy_info.time_ms += duration;
          }
        }
        status = AUDMAIN_STATUS_SUCCESS;
      }
      break;

    case 'b': /* <basic-note> */
      if(audimy_parse_backlight(&is_note)) {
        if(is_note == FALSE) {
          /* backlight */
          status = AUDMAIN_STATUS_SUCCESS;
        }
        else {
          /* Is actually note 'b' */
          if(audimy_process_note()) {
            status = AUDMAIN_STATUS_SUCCESS;
          }
        }
      }
      break;

    case 'e':  /* <basic-note>    */
      /* Check if this is the END */
      if(audimy_parse_end(&is_note)) {
        if(is_note == FALSE) {
          /* Done */
          audimy_done_parsing = TRUE;
          if( audimy_curr_state == AUDIMY_STATE_CALC_TIME){
            audimy_curr_state = AUDIMY_STATE_NONE;
          }
          return AUDMAIN_STATUS_DONE;
        }
        else {
          /* Is actually note 'b' */
          if(audimy_process_note()) {
            status = AUDMAIN_STATUS_SUCCESS;
          }
        }
      }
      break;

    case '*':  /* <octave-prefix> */
    case '&':  /* <ess-note>      */
    case '#':  /* <iss-note>      */

    case 'c':  /* <basic-note>    */
    case 'd':
    case 'f':
    case 'g':
    case 'a':
      if(audimy_process_note()) {
        /* Play note */
        status = AUDMAIN_STATUS_SUCCESS;
      }
      break;

    case '(':
      /* Start of a loop */
      if(audimy_loop_found == TRUE) {
        MSG_ERROR("Can't support nested loops", 0,0,0);
        return AUDMAIN_STATUS_DATA_ERR;
      }
      audimy_loop_found = TRUE;
      audimy_num_toks_in_loop = -1;
      audimy_loop_time = 0;

      /* Skip the '(' char., the next time we repeat this loop. */
      audimy_update_data_index(1);
      status = AUDMAIN_STATUS_SUCCESS;
      break;

    case '@':
      /* End of loop */
      if(audimy_parse_loop_end()) {
        if(audimy_data_state == AUDIMY_DATA_READY) {
          /* Break out of the while loop, so that the buffers can be reloaded
           * to repeat the loop */
          return AUDMAIN_STATUS_SUCCESS;
        }
      }
      break;

    case 'E':
      /* End of melody */
      if(audimy_parse_end(&is_note)) {
        if(is_note == TRUE) {
          /* There is no 'E' note. Only 'e' note is valid */
          MSG_ERROR("Unexpected char. '%c' in input", buffer[0], 0, 0);
          return AUDMAIN_STATUS_DATA_ERR;
        }
        else {
          if(audimy_loop_found) {
            MSG_ERROR("Unexpected end of loop", 0,0,0);
            return AUDMAIN_STATUS_DATA_ERR;
          }
          /* Done */
          mel_cmd_buf[0] = 0;
          audimy_add_mel_cmd(AUDIMY_MEL_END, mel_cmd_buf);

          audimy_done_parsing = TRUE;
          if( audimy_curr_state == AUDIMY_STATE_CALC_TIME){
            audimy_curr_state = AUDIMY_STATE_NONE;
          }
          return AUDMAIN_STATUS_SUCCESS;
        }
      }
      break;

    default:
      MSG_ERROR("Unexpected char. '%c' in input", buffer[0], 0, 0);
    }

    switch(audimy_data_state) {
    case AUDIMY_DATA_ERROR:
      return audimy_pp_ctl.error_status;

    case AUDIMY_DATA_READY:
      if(status != AUDMAIN_STATUS_SUCCESS) {
        MSG_ERROR("Data error", 0,0,0);
        return AUDMAIN_STATUS_DATA_ERR;
      }
      break;

    default:
      /* AUDIMY_DATA_LOADING || AUDIMY_DATA_FLUSHING */
      /* Wait until data operation completes */
      return AUDMAIN_STATUS_SUCCESS;
    }

    audimy_num_toks_in_loop++;
    status = AUDMAIN_STATUS_ERROR;
  }
  return AUDMAIN_STATUS_SUCCESS;
}

/* <EJECT> */
/*===========================================================================

FUNCTION audimy_age

DESCRIPTION
  If we have finished playing the current note, it issues the command
  to play the next note.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sets audimy_done_playing to TRUE after the last note is played.

===========================================================================*/
void audimy_age (
  audmain_parse_ctl_type  *parse_ctl,
  uint32                  *elapsed_time,
  uint32                  *adjusted_time
)
{
  word                      *buffer;
  audmain_cmd_buf_type      *cmd_buf = &parse_ctl->current_buf;
  snd_af_access_type        *handle  = audimy_pp_ctl.handle;
  audmain_midi_cmd_buf_type *midi_buf;
  snd_cmx_af_led_ctl_type   led_ctl;
  int32                     excess_time;
  byte                      cmd_index = 0;
  byte                      cmd_length = 0;

  midi_buf = &cmd_buf->midi_cmd[0];
  audimy_curr_state = AUDIMY_STATE_PLAYING;
  *adjusted_time = *elapsed_time;

  if(!(parse_ctl->status_flag & AUDMAIN_MASK_INIT_DONE)) {
    *elapsed_time  = 0;
    *adjusted_time = 0;
  }
  else {
    audimy_elapsed_time += *elapsed_time;
    if(audimy_elapsed_time >= audimy_note_length) {
      if(audimy_note_length) {
        audimy_excess_elapsed_time = audimy_elapsed_time - audimy_note_length;
        if(audimy_excess_elapsed_time < 0) {
          audimy_excess_elapsed_time = 0;
        }
      }
      audimy_elapsed_time    = 0;
      midi_buf->num_words    = 0;
      midi_buf->num_commands = 0;
      /* Play the next note */
      while(audimy_done_playing == FALSE &&
            cmd_index < AUDMAIN_CMD_BUF_MAX_WORDS &&
            audimy_get_mel_cmd(&buffer) == TRUE) {

        cmd_length = audimy_cmd_len[ buffer[0] ];

        switch(buffer[0]) {
        case AUDIMY_MEL_VOLUME:
          /* Convert it back to the iMelody scale (for DTMF/ringer volumes) */
          audimy_current_volume = buffer[4]? ((buffer[4] - 37) / 6) : 0;
          /* Fall thru */
        case AUDIMY_MEL_RESET:
        case AUDIMY_MEL_PROGRAM:
          midi_buf->num_words    += cmd_length;
          midi_buf->num_commands += 1;
          (void)memcpy(&midi_buf->command_buf[cmd_index],
                       &buffer[1], cmd_length*2);
          break;

        case AUDIMY_MEL_BACKLIGHT:
          MSG_MED("Backlight state %d", buffer[1], 0,0);
          handle->data_func(SND_CMX_AF_BACKLIGHT_CONTROL, handle->audfmt_ptr,
                            0, &buffer[1]);
          cmd_length = 0;
          break;

        case AUDIMY_MEL_LED:
          MSG_MED("LED state %d", buffer[1], 0,0);
          led_ctl.led_ctl = SND_CMX_AF_LED_BRIGHT_YELLOW;
          led_ctl.led_enable = buffer[1];
          handle->data_func(SND_CMX_AF_LED_CONTROL, handle->audfmt_ptr, 0,
                            &led_ctl);
          cmd_length = 0;
          break;

        case AUDIMY_MEL_VIBRATOR:
          MSG_MED("Vibrator state %d", (int)buffer[1], 0,0);
          cmd_buf->vib = buffer[1];
          cmd_length = 0;
          break;

        case AUDIMY_MEL_NOTE_ON:
#ifdef AUDIMY_DEBUG
#error code not present
#endif
          cmd_buf->imy_volume = audimy_current_volume;

        case AUDIMY_MEL_NOTE_OFF:

          /* Remove the additional length from the duration field */
          cmd_length--;

          /* If the previous note played for a longer period than its
           * assigned length, maintain the correct tempo by reducing the length
           * of next note(s)
           */
          if(audimy_excess_elapsed_time > 0) {
            if(buffer[1]) {
              excess_time = buffer[1] - audimy_excess_elapsed_time;
              if(excess_time <= 0) {
                /* skip note */
                audimy_excess_elapsed_time += excess_time;
                audimy_note_length = 0;
                if(buffer[0] == AUDIMY_MEL_NOTE_ON || !audimy_note_is_on) {
                  break;
                }
                excess_time = 0;
              }
              audimy_note_length = excess_time;
            }
            else {
              audimy_note_length = 0;
            }
          }
          else {
            audimy_note_length = buffer[1];
          }

          midi_buf->num_words    += cmd_length;
          midi_buf->num_commands += 1;
          (void)memcpy(&midi_buf->command_buf[cmd_index],
                       &buffer[2], cmd_length*2);

          if(buffer[0] == AUDIMY_MEL_NOTE_ON) {
            audimy_note_is_on = TRUE;
          }
          else {
            audimy_note_is_on = FALSE;
          }

          /* Play note with non-zero length now */
          if(audimy_note_length) {
            return;
          }
          /* Queue up zero length commands */
          break;

        default:
          MSG_ERROR("Unknown AUDIMY_MEL_TYPE %d", buffer[0],0,0);
          /* FALL THRU */

        case AUDIMY_MEL_END:
          /* Ensure that we restore every thing, in case the
           * song didn't do it already */
          cmd_buf->vib = FALSE;
          audimy_done_playing = TRUE;
          break;

        }

        cmd_index += cmd_length;
      } /* while(..) */
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION audimy_do_time

DESCRIPTION
 Calculates the time in ms for playing the file.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_DONE -Success
  AUDMAIN_STATUS_PENDING - Pending
  AUDMAIN_STATUS_ERROR

SIDE EFFECTS
  audimy_info.time_ms
===========================================================================*/
audmain_status_type audimy_do_time (
  audmain_parse_ctl_type  *parse_ctl,
  uint32                  *elapsed_time
 ){
  audmain_cmd_buf_type    *cmd_buf;
  uint32                  time_remain;
  audmain_status_type     status;

  if(audimy_curr_state == AUDIMY_STATE_PLAYING) {
    *elapsed_time = audimy_info.time_ms;
    return AUDMAIN_STATUS_DONE;
  }

  if(parse_ctl) {
    cmd_buf = &parse_ctl->current_buf;
    time_remain = 0;
    audimy_curr_state = AUDIMY_STATE_CALC_TIME;

    while((audimy_data_state != AUDIMY_DATA_LOADING) && !audimy_done_parsing) {
      /* Continue with calculating time */
      status = audimy_parse(parse_ctl, cmd_buf, &time_remain);
      if((status == AUDMAIN_STATUS_ERROR) ||
         (status == AUDMAIN_STATUS_DATA_ERR) ||
         (status == AUDMAIN_STATUS_DATA_ACCESS_ERR) ||
         (status == AUDMAIN_STATUS_MAX)) {

        /* If status is MAX, error status is not properly set. */
        return AUDMAIN_STATUS_ERROR;
      }

      /* Simulate buffer consumption */
      audimy_cmd_buf_length = 0;
      audimy_cmd_index      = 0;
    }

    /* Done calculating time */
    if(audimy_done_parsing) {
      *elapsed_time = audimy_info.time_ms;
      audimy_reset();
      return AUDMAIN_STATUS_DONE;
    }
    else{
      return AUDMAIN_STATUS_PENDING;
    }
  }

  return AUDMAIN_STATUS_ERROR;
}

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
void audimy_pause (
  audmain_parse_ctl_type *parse_ctl,
  boolean                pause_flag
) {
  if(parse_ctl != NULL) {
    if (pause_flag) {
#ifdef FEATURE_MIDI
      /* Pause the iMelody stream that is playing on the Qsynth/MIDI */
      audmain_audfmt_pause_seq(0, TRUE);
#endif /* FEATURE_MIDI */

      /* Temporarily stop the vibration */
      parse_ctl->current_buf.vib = FALSE;
    }
    else {
      /* Resume the vibration to the previous state */
      parse_ctl->current_buf.vib = audimy_pp_ctl.vib_en;

#ifdef FEATURE_MIDI
      /* Resume the iMelody stream */
      audmain_audfmt_pause_seq(0, FALSE);
#endif /* FEATURE_MIDI */
    }
  }
} /* end audimy_pause */
#endif /* FEATURE_IMELODY */

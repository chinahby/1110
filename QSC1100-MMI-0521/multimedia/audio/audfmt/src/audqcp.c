/* linesize(132)
** pagesize(60)
** title("Dual Mode Subscriber Station")
** subtitle("Audio Formats Services")
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             Q C P   A U D I O   F O R M A T s   S E R V I C E S

GENERAL DESCRIPTION
  These functions implement a QCP file format player.

EXTERNALIZED FUNCTIONS
  audqcp_get_data_cb_req
    Handles the get/read data callback.
  audqcp_parse_head
    Parses the head of a QCP file.
  audqcp_age
    Ages the specified QCP file by a number of milliseconds.
  audqcp_parse
   Parses one event of the indicated QCP file.
  audqcp_fforward
    Moves the QCP playback forward ms milliseconds.
  audqcp_rewind
    Moves the QCP playback backward ms milliseconds.
  audqcp_do_seek
    Moves the QCP playback to the seek ms milliseconds.
 
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright(c) 2001 - 2007 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/audfmt/main/latest/src/audqcp.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/29/08    sj     Added .awb file format support
10/26/07    hs     Added EVW file format support
10/01/07   ymc     Initialized frame_size to 0 in audqcp_count_frames.
07/05/07    hs     Fixed CR 122000 checking invalid headers in qcp file format
04/20/07    at     Added EVRC-B file format recording support.
03/21/07    at     Added EVRC-B file format playback support.
10/13/06    az     Modified the code to fix CR: 102435 Reset in AMR with incorrect 
                   last frame
10/12/06    kan    Fixed GUID and Name for 4GV NB Header.
06/15/06    az     Fixed CR 81282 Error recording voice memo having 
                   AMR audio format during a voice call.  
11/14/05    ay     Modified code to prevent seeking past the end time.
10/27/05    pl     Modified audqcp_get_data_cb_req() to handle corrupted audio 
                   data.
09/18/05    st     Modified code to use qcpsup_validate_raw_frames() to check
                   for headerless QCP files.
09/01/05  aw/ay    Modified code to handle reading corrupted QCP files CR74010.
08/31/05    sm     Initialized seek_frames parameter to fix coverity error.
08/24/05   ayu     Modified code to delete recorded QCP files containing only
                   header information, for CR62770.
08/11/05    hz     Added support for DTX and data requrest time for AMR 
                   recording.
08/11/05    lg     Fixed typo in previous version.
08/05/05    lg     Removed check for 0 length buffers, NULL buffers indicate error.
08/04/05    sp     Added support for mixing of qcp audio and voice call.
07/19/05    pl     Added the support for setting the error status at the point 
                   where the error is encountered. The error status would 
                   eventually be relayed back to the upper layer to provide
                   more information for the aborted playback.
07/08/05    pl     Modified audqcp_do_file_spec to set supported operation
                   bit mask.
04/28/05    aw     Modified code to properly handle elapsed frame is equal to 
                   the frist frame in audqcp_alocated_ms().
                   Modified code to properly calculated the total time.
04/28/05   ymc     Added 4GV support.
09/15/04    sm     Added QCP file support for EFR/FR/HR vocoders.
08/11/04    lg     Modified to clear num_frames - will force it be re-evaluated
                   everytime get_time is invoked.
01/29/04    aw     Modified code in audqcp_restart to not request new buffer
                   if there is a buffer to skip.
01/26/04  aw/sg    Added support for getting file spec. Fixed syntax errors.
01/22/04    lg     Added get_time functionality
01/15/04    aw     Reset the status_flag in audqcp_format_play.
12/08/03    aw     Lint update; Added check for 0 data length.
10/16/03  aw/st    Removed the capability to recognize raw AMR frames.
                   Modified code to do audqcp_flush().
09/27/03    st     Modified code to set playback parameters properly.
09/25/03    st     Modified to not queue empty frames to the vocoder for AMR.
09/19/03    st     Modified to not queue empty frames to the vocoder.
04/17/03    sm     Added AMR file format support.
01/25/03    st     Modified code to properly handle status from first call to
                   audqcp_seek_frames() in audqcp_do_seek().
11/19/02    st     Modified code to return BUFF_ERR status if both QCP buffers
                   are pending.
11/06/02    sm     Added support for AMR vocoder.
08/13/02    yl     Modified code in audqcp_record_stop() to handle the 
                   AUDMAIN_REC_AUTO_STOP status. 
08/10/02  aw/st    Modified code to use updated qcpsup interface; changed
                   default size fields in QCP files to zero.
07/25/02    st     Added audqcp_format_play() to play files with no headers
                   and audqcp_flush().
06/11/02    st     Modified code to handle recording of fixed fullrate EVRC
                   files.
09/19/01    st     Modified audqcp_evrc_header[] to have a valid size for
                   quarter rate EVRC frames.
09/16/01    st     Added support for setting cnfg chunk.                 
09/03/01  st/aw    Modified code to support EVRC QCP playback; added code
                   to support ff/rewind for playback; added code to support
                   QCP recording; modified code to play back to reverse link;
                   added code to support QCP functions (get/set chunk info).
04/04/01    st     Created

===========================================================================*/
/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "task.h"               /* Task definitions and prototypes         */

#ifdef FEATURE_QCP
#include "audqcp.h"             /* Audio Format definitions and prototypes */
#include "audqcpi.h"            /* Internal Audio Format definitions       */
#include "audmain.h"            /* Audio Format definitions and prototypes */
#include "msg.h"                /* Message logging/reporting services      */
#include "err.h"                /* Error logging/reporting services        */
#include <string.h>             /* String routines                         */
#include <math.h>               /* Math routines                           */

/* <EJECT> */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

There are definitions for LOCAL data.  There is no global data.

===========================================================================*/

/* This structure contains all the information about a file
** that is being parsed. This module currently only supports parsing
** one file at a time.
*/
audqcp_parse_ctl_type audqcp_parse_ctl;

audqcp_buf_mgmt_type audqcp_buf_mgmt;

/* This variable holds the size of the data buffers being returned by
** the client.
*/
uint32 audqcp_buf_size;

/* These variables are the QCP recording callback function and client
** data pointer. These are used to update the client with any
** recording status.
*/
snd_qcp_rec_cb_func_ptr_type audqcp_rec_cb       = NULL;
const void                   *audqcp_client_data = NULL;

/* The following variables are used to keep track of the current QCP 
** recording session.
*/
qcpsup_format_enum_type  audqcp_rec_format;     /* Format being recorded     */
snd_qcp_dir_type         audqcp_rec_link;       /* Path being recorded       */
uint32  audqcp_rec_rx_bytes;   /* Number of bytes recorded on forward link   */
uint32  audqcp_rec_rx_frames;  /* Number of frames recorded on forward link  */
uint32  audqcp_rec_tx_bytes;   /* Number of bytes recorded on reverse link   */
uint32  audqcp_rec_tx_frames;  /* Number of frames recorded on reverse link  */
uint32  audqcp_rec_head_bytes; /* Number of bytes in header of recorded file */

/* Size of header for QCP files is 194 bytes
*/
#define AUDQCP_HEADER_SIZE  194

/* Generic header for 13K QCP file. This data array is used in recording
** 13k QCP files.
*/
const uint8 audqcp_13k_header [] = {
  'R', 'I', 'F', 'F',
  0x00,0x00,0x00,0x00,   /* Offset 4: file length, to be filled in    */
  'Q', 'L', 'C', 'M', 'f', 'm', 't', ' ',
  0x96, 0x00, 0x00, 0x00,
  0x01,0x00,
  0x41,0x6D,0x7F,0x5E,
  0x15,0xB1,
  0xD0,0x11,
  0xBA,0x91,0x00,0x80,0x5F,0xB4,0xB9,0x7E,
  0x02,0x00,
  0x51,0x63,0x65,0x6C,0x70,0x20,0x31,0x33,
  0x4B,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0xC8,0x32,
  0x23,0x00,
  0xA0,0x00,
  0x40,0x1F,
  0x10,0x00,
  0x05,0x00,0x00,0x00,
  0x22,0x04,0x10,0x03,0x07,0x02,0x03,0x01,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,
  'v', 'r', 'a', 't',
  0x08,0x00,0x00,0x00,0x01,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,   /* Offset 182: packet length to be filled in */
  'd', 'a', 't', 'a',
  0x00,0x00,0x00,0x00,   /* Offset 190: data length, to be filled in  */

};

/* Generic header for EVRC QCP file. This data array is used in recording
** EVRC QCP files.
*/
const uint8 audqcp_evrc_header [] = {
  'R', 'I', 'F', 'F',
  0x00,0x00,0x00,0x00,   /* Offset 4: file length, to be filled in    */
  'Q', 'L', 'C', 'M', 'f', 'm', 't', ' ',
  0x96, 0x00, 0x00, 0x00,
  0x01,0x00,
  0x8D,0xD4,0x89,0xE6,
  0x76,0x90,
  0xB5,0x46,
  0x91,0xEF,0x73,0x6A,0x51,0x00,0xCE,0xB4,
  0x01,0x00,
  0x54,0x49,0x41,0x20,0x49,0x53,0x2D,0x31,
  0x32,0x37,0x20,0x45,0x6E,0x68,0x61,0x6E,
  0x63,0x65,0x64,0x20,0x56,0x61,0x72,0x69,
  0x61,0x62,0x6C,0x65,0x20,0x52,0x61,0x74,
  0x65,0x20,0x43,0x6F,0x64,0x65,0x63,0x2C,
  0x20,0x53,0x70,0x65,0x65,0x63,0x68,0x20,
  0x53,0x65,0x72,0x76,0x69,0x63,0x65,0x20,
  0x4F,0x70,0x74,0x69,0x6F,0x6E,0x20,0x33,
  0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0xC8,0x32,
  0x16,0x00,
  0xA0,0x00,
  0x40,0x1F,
  0x10,0x00,
  0x05,0x00,0x00,0x00,
  0x16,0x04,0x0A,0x03,0x05,0x02,0x02,0x01,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,
  'v', 'r', 'a', 't',
  0x08,0x00,0x00,0x00,0x01,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,   /* Offset 182: packet length to be filled in */
  'd', 'a', 't', 'a',
  0x00,0x00,0x00,0x00,   /* Offset 190: data length, to be filled in  */

};

const uint8 audqcp_4gv_nb_header [] = {
  'R', 'I', 'F', 'F',
  0x00,0x00,0x00,0x00,   /* Offset 4: file length, to be filled in    */
  'Q', 'L', 'C', 'M', 'f', 'm', 't', ' ',
  0x96, 0x00, 0x00, 0x00,
  0x01,0x00,
  0xCA,0x29,0xFD,0x3C,                      /* GUID Start */
  0x53,0xF6,
  0xF5,0x4E,
  0x90,0xE9,
  0xF4,0x23,0x6D,0x59,0x9B,0x61,            /* GUID END */
  0x01,0x00,                                /* VERSION */
  0x46,0x6F,0x75,0x72,0x74,0x68,0x20,0x47,  /* NAME START */
  0x65,0x6E,0x65,0x72,0x61,0x74,0x69,0x6F,
  0x6E,0x20,0x56,0x6F,0x63,0x6F,0x64,0x65,
  0x72,0x20,0x28,0x34,0x47,0x56,0x29,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* NAME END 80 Bytes */
  0xC8,0x32,
  0x16,0x00,
  0xA0,0x00,
  0x40,0x1F,
  0x10,0x00,
  0x05,0x00,0x00,0x00,
  0x16,0x04,0x0A,0x03,0x05,0x02,0x02,0x01,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,
  'v', 'r', 'a', 't',
  0x08,0x00,0x00,0x00,0x01,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,   /* Offset 182: packet length to be filled in */
  'd', 'a', 't', 'a',
  0x00,0x00,0x00,0x00,   /* Offset 190: data length, to be filled in  */

};

/* Generic header for AMR QCP file. This data array is used in recording
** AMR QCP files.
*/
const uint8 audqcp_amr_header [] = {
  'R', 'I', 'F', 'F',
  0x00,0x00,0x00,0x00,   /* Offset 4: file length, to be filled in    */
  'Q', 'L', 'C', 'M', 'f', 'm', 't', ' ',
  0x96, 0x00, 0x00, 0x00,
  0x01,0x00,
  0x53,0xE0,0xA8,0x6A,
  0x4F,0x47,
  0x46,0xBD,
  0x8A,0xFA,
  0xAC,0xF2,0x32,0x82,0x73,0xBD,
  0x01,0x00,
  0x41,0x4D,0x52,0x20,0x41,0x4D,0x52,0x20, /* Begin ascii description block */
  0x41,0x4D,0x52,0x20,0x41,0x4D,0x52,0x20,
  0x41,0x4D,0x52,0x20,0x41,0x4D,0x52,0x20,
  0x41,0x4D,0x52,0x20,0x41,0x4D,0x52,0x20,
  0x41,0x4D,0x52,0x20,0x41,0x4D,0x52,0x20,
  0x41,0x4D,0x52,0x20,0x41,0x4D,0x52,0x20,
  0x41,0x4D,0x52,0x20,0x41,0x4D,0x52,0x20,
  0x41,0x4D,0x52,0x20,0x41,0x4D,0x52,0x20,
  0x41,0x4D,0x52,0x20,0x41,0x4D,0x52,0x20,
  0x41,0x4D,0x52,0x20,0x41,0x4D,0x52,0x00, /* End ascii description block   */
  0xC8,0x32,          /* Bits per second */
  0x16,0x00,          /* Bytes per packet */
  0xA0,0x00,          /* Samples per block */
  0x40,0x1F,          /* Samples per second */
  0x10,0x00,          /* Bits per sample */
  0x05,0x00,0x00,0x00,
  0x16,0x04,0x0A,0x03,0x05,0x02,0x02,0x01,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,
  'v', 'r', 'a', 't',
  0x08,0x00,0x00,0x00,0x01,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,   /* Offset 182: packet length to be filled in */
  'd', 'a', 't', 'a',
  0x00,0x00,0x00,0x00,   /* Offset 190: data length, to be filled in  */

};

/* Generic header for EFR QCP file. This data array is used in recording
** EFR QCP files.
*/
const uint8 audqcp_efr_header [] = {
  'R', 'I', 'F', 'F',
  0x00,0x00,0x00,0x00,   /* Offset 4: file length, to be filled in    */
  'Q', 'L', 'C', 'M', 'f', 'm', 't', ' ',
  0x96, 0x00, 0x00, 0x00,
  0x01,0x00,
  0x08,0x90,0x00,0x75,
  0x89,0xe9,
  0x4b,0x6b,
  0x93,0x1f,
  0x5c,0x0e,0x67,0xad,0x6d,0x5d,
  0x01,0x00,
  0x45,0x46,0x52,0x20,0x45,0x46,0x52,0x20, /* Begin ascii description block */
  0x45,0x46,0x52,0x20,0x45,0x46,0x52,0x20,
  0x45,0x46,0x52,0x20,0x45,0x46,0x52,0x20,
  0x45,0x46,0x52,0x20,0x45,0x46,0x52,0x20,
  0x45,0x46,0x52,0x20,0x45,0x46,0x52,0x20,
  0x45,0x46,0x52,0x20,0x45,0x46,0x52,0x20,
  0x45,0x46,0x52,0x20,0x45,0x46,0x52,0x20,
  0x45,0x46,0x52,0x20,0x45,0x46,0x52,0x20,
  0x45,0x46,0x52,0x20,0x45,0x46,0x52,0x20,
  0x45,0x46,0x52,0x20,0x45,0x46,0x52,0x00, /* End ascii description block   */
  0xC8,0x32,
  0x16,0x00,
  0xA0,0x00,
  0x40,0x1F,
  0x10,0x00,
  0x05,0x00,0x00,0x00,
  0x16,0x04,0x0A,0x03,0x05,0x02,0x02,0x01,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,
  'v', 'r', 'a', 't',
  0x08,0x00,0x00,0x00,0x01,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,   /* Offset 182: packet length to be filled in */
  'd', 'a', 't', 'a',
  0x00,0x00,0x00,0x00,   /* Offset 190: data length, to be filled in  */

};

/* Generic header for FR QCP file. This data array is used in recording
** FR QCP files.
*/
const uint8 audqcp_fr_header [] = {
  'R', 'I', 'F', 'F',
  0x00,0x00,0x00,0x00,   /* Offset 4: file length, to be filled in    */
  'Q', 'L', 'C', 'M', 'f', 'm', 't', ' ',
  0x96, 0x00, 0x00, 0x00,
  0x01,0x00,
  0x8c,0x3d,0x5c,0xe8,
  0x8b,0xf0,
  0x48,0xa5,
  0x85,0x46,
  0x18,0x87,0xeb,0x2b,0x4d,0x05,
  0x01,0x00,
  0x20,0x46,0x52,0x20,0x20,0x46,0x52,0x20, /* Begin ascii description block */
  0x20,0x46,0x52,0x20,0x20,0x46,0x52,0x20,
  0x20,0x46,0x52,0x20,0x20,0x46,0x52,0x20,
  0x20,0x46,0x52,0x20,0x20,0x46,0x52,0x20,
  0x20,0x46,0x52,0x20,0x20,0x46,0x52,0x20,
  0x20,0x46,0x52,0x20,0x20,0x46,0x52,0x20,
  0x20,0x46,0x52,0x20,0x20,0x46,0x52,0x20,
  0x20,0x46,0x52,0x20,0x20,0x46,0x52,0x20,
  0x20,0x46,0x52,0x20,0x20,0x46,0x52,0x20,
  0x20,0x46,0x52,0x20,0x20,0x46,0x52,0x00, /* End ascii description block   */
  0xC8,0x32,
  0x16,0x00,
  0xA0,0x00,
  0x40,0x1F,
  0x10,0x00,
  0x05,0x00,0x00,0x00,
  0x16,0x04,0x0A,0x03,0x05,0x02,0x02,0x01,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,
  'v', 'r', 'a', 't',
  0x08,0x00,0x00,0x00,0x01,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,   /* Offset 182: packet length to be filled in */
  'd', 'a', 't', 'a',
  0x00,0x00,0x00,0x00,   /* Offset 190: data length, to be filled in  */

};

/* Generic header for HR QCP file. This data array is used in recording
** HR QCP files.
*/
const uint8 audqcp_hr_header [] = {
  'R', 'I', 'F', 'F',
  0x00,0x00,0x00,0x00,   /* Offset 4: file length, to be filled in    */
  'Q', 'L', 'C', 'M', 'f', 'm', 't', ' ',
  0x96, 0x00, 0x00, 0x00,
  0x01,0x00,
  0x10,0x36,0x66,0x1e,
  0xdb,0x73,
  0x42,0x04,
  0x86,0x97,
  0xf9,0x14,0xd3,0xd8,0x6d,0x8c,
  0x01,0x00,
  0x20,0x48,0x52,0x20,0x20,0x48,0x52,0x20, /* Begin ascii description block */
  0x20,0x48,0x52,0x20,0x20,0x48,0x52,0x20,
  0x20,0x48,0x52,0x20,0x20,0x48,0x52,0x20,
  0x20,0x48,0x52,0x20,0x20,0x48,0x52,0x20,
  0x20,0x48,0x52,0x20,0x20,0x48,0x52,0x20,
  0x20,0x48,0x52,0x20,0x20,0x48,0x52,0x20,
  0x20,0x48,0x52,0x20,0x20,0x48,0x52,0x20,
  0x20,0x48,0x52,0x20,0x20,0x48,0x52,0x20,
  0x20,0x48,0x52,0x20,0x20,0x48,0x52,0x20,
  0x20,0x48,0x52,0x20,0x20,0x48,0x52,0x00, /* End ascii description block   */
  0xC8,0x32,
  0x16,0x00,
  0xA0,0x00,
  0x40,0x1F,
  0x10,0x00,
  0x05,0x00,0x00,0x00,
  0x16,0x04,0x0A,0x03,0x05,0x02,0x02,0x01,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,
  'v', 'r', 'a', 't',
  0x08,0x00,0x00,0x00,0x01,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,   /* Offset 182: packet length to be filled in */
  'd', 'a', 't', 'a',
  0x00,0x00,0x00,0x00,   /* Offset 190: data length, to be filled in  */

};

/* Buffer containing the cnfg chunk ID, used to add the cnfg chunk
** to QCP files.
*/
const uint8 audqcp_cnfg_chunk [] = {
  'c', 'n', 'f', 'g'
};

/* Buffer containing the labl chunk ID, used to add the labl chunk
** to QCP files.
*/
const uint8 audqcp_labl_chunk [] = {
  'l', 'a', 'b', 'l'
};

/* Buffer containing the text chunk ID, used to add the text chunk
** to QCP files.
*/
const uint8 audqcp_text_chunk [] = {
  't', 'e', 'x', 't'
};

/* The following parameters are used when executing a command to get
** information from a QCP file.
*/
snd_af_access_type *audqcp_get_chunk_handle   = NULL;
uint32             audqcp_get_chunk_length    = 0;

audqcp_func_type   audqcp_func_status    = AUDQCP_FUNC_INVALID;
snd_af_access_type *audqcp_func_handle   = NULL;
uint32             audqcp_func_file_size = 0;
uint8              *audqcp_func_buffer   = NULL;

/* The following parameters are used when executing a command to set
** the labl chunk in a QCP file.
*/
snd_qcp_rec_cb_func_ptr_type audqcp_set_chunk_cb_func      = NULL;
const void                   *audqcp_set_chunk_client_data = NULL;
uint8                        *audqcp_set_chunk_buffer_ptr  = NULL;
uint32                       audqcp_set_chunk_num_bytes;

/* This buffer is used to ensure that a fixed size of 48 bytes is written
** to the QCP file.
*/
#define AUDQCP_LABL_SIZE 48
uint8 audqcp_set_labl_buffer[AUDQCP_LABL_SIZE];

/* This variable is used as the pad byte in creating any odd length chunks
*/
const uint8 audqcp_pad_byte = 0;

/* Maximum unsigned 32-bit value; mainly used to indicate that length of
** QCP file is indeterminant.
*/
#define AUDQCP_MAX_32BIT           ((unsigned) 0xFFFFFFFF)

#ifdef FEATURE_AUDFMT_EVB
/* EVRC-B file header. */
const uint8 evb_id[] = {'#', '!', 'E', 'V', 'R', 'C', '-', 'B', '\n'};

/* EVRC-B file header size */
#define AUDEVB_HEADER_SIZE sizeof(evb_id)
#endif /* FEATURE_AUDFMT_EVB */


#ifdef FEATURE_AUDFMT_EVW
/*EVRC WB - magic number*/
/*"#!EVCWB\n"*/
/*Equivalent value = "0x23 0x21 0x45 0x56 0x43 0x57 0x42 0x0A"*/
const uint8 evw_id[] = {'#', '!', 'E', 'V', 'C', 'W', 'B','\n'};
#define AUDEVW_HEADER_SIZE sizeof(evw_id)
#endif

/* Headers for AMR-WB files of various configurations.  This data array is used in
** format detectionof .awb files.
*/
/* Single channel wideband AMR */
const uint8 amr_wb_id[] = {'#','!','A','M','R','-','W','B','\n'};

/* Multichannel wideband AMR */
const uint8 amr_wb_multi_id[] =
                 {'#','!','A','M','R','-','W','B','_','M','C','1','.','0','\n'};

/* Size of header for AMR files is dependent on the configuration.
*/
#define AUDAMR_WB_HEADER_SIZE(x) sizeof(x)

/* Defines to extract the number of channels from a multichannel AMR file */
#define AUDAMR_WB_CHANNEL_MASK 0xF0000000
#define AUDAMR_WB_CHANNEL_SHIFT 28


#ifdef FEATURE_AUDFMT_AMR
/* Headers for AMR files of various configurations.  This data array is used in
** format detection and recording .AMR files.
*/
const uint8 amr_id[] = {'#','!','A','M','R','\n'};

/* Multichannel AMR */ 
const uint8 amr_multi_id[] = {'#','!','A','M','R','_','M','C','1','.','0','\n'};

/* Size of header for AMR files is dependent on the configuration.
*/
#define AUDAMR_HEADER_SIZE(x) sizeof(x)

/* Defines to extract the number of channels from a multichannel AMR file */
#define AUDAMR_CHANNEL_MASK 0xF0000000
#define AUDAMR_CHANNEL_SHIFT 28

/* <EJECT> */
/*===========================================================================

FUNCTION audqcp_amr_extract_file_header

DESCRIPTION
  This function extracts the header and returns the format of the AMR file.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_SUCCESS or _FAILURE.
  Config of AMR file, including format, number of channels, starting index, etc.

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audqcp_amr_extract_file_header (
  uint8                   *buffer,
  uint32                  *index,
  audqcp_parse_ctl_type   *parse_ctl
) {
  audmain_status_type status = AUDMAIN_STATUS_FAILURE;
  
  if(memcmp(amr_id, buffer, AUDAMR_HEADER_SIZE(amr_id)) == 0) {
    /* Standard AMR file */
    parse_ctl->wideband_flag = FALSE;
    parse_ctl->num_channels  = 1;
    *index                   = *index + AUDAMR_HEADER_SIZE(amr_id);
    status                   = AUDMAIN_STATUS_SUCCESS;
  } else if(memcmp(amr_multi_id, buffer,
                                       AUDAMR_HEADER_SIZE(amr_multi_id)) == 0) {
    /* Standard multichannel AMR file */
    parse_ctl->wideband_flag = FALSE;
    parse_ctl->num_channels  = (
                           (uint32)(buffer + AUDAMR_HEADER_SIZE(amr_multi_id)) &
                                   AUDAMR_CHANNEL_MASK) >> AUDAMR_CHANNEL_SHIFT;  
    *index                   = *index + AUDAMR_HEADER_SIZE(amr_multi_id) + 4;
    status                   = AUDMAIN_STATUS_ERROR;
  } 

  return(status);
}
#endif /* FEATURE_AUDFMT_AMR */

/* <EJECT> */
/*===========================================================================

FUNCTION audqcp_amr_wb_extract_file_header

DESCRIPTION
  This function extracts the header and returns the format of the AWB file.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_SUCCESS or _FAILURE.
  Config of AWB file, including format, number of channels, starting index, etc.

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audqcp_amr_wb_extract_file_header (
  uint8                   *buffer,
  uint32                  *index,
  audqcp_parse_ctl_type   *parse_ctl
) {
  audmain_status_type status = AUDMAIN_STATUS_FAILURE;
  
  if(memcmp(amr_wb_id, buffer, AUDAMR_WB_HEADER_SIZE(amr_wb_id)) == 0) {
    /* Wideband AMR file */
    parse_ctl->wideband_flag = TRUE;
    parse_ctl->num_channels  = 1;
    *index                   = *index + AUDAMR_WB_HEADER_SIZE(amr_wb_id);
    status                   = AUDMAIN_STATUS_SUCCESS;
  } else if(memcmp(amr_wb_multi_id, buffer,
                   AUDAMR_WB_HEADER_SIZE(amr_wb_multi_id)) == 0) {
    /* Wideband multichannel AMR file */
    parse_ctl->wideband_flag = TRUE;
    parse_ctl->num_channels  = (
                 (uint32)(buffer + AUDAMR_WB_HEADER_SIZE(amr_wb_multi_id)) & \
                           AUDAMR_WB_CHANNEL_MASK) >> AUDAMR_WB_CHANNEL_SHIFT;
    *index                   = *index + AUDAMR_WB_HEADER_SIZE(amr_wb_multi_id)\
                                + 4;
    status                   = AUDMAIN_STATUS_ERROR;
  }
  return(status);
}


#ifdef FEATURE_AUDFMT_EVB
/* <EJECT> */
/*===========================================================================

FUNCTION audqcp_evb_extract_file_header

DESCRIPTION
  This function extracts the header of the EVRC-B file.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_SUCCESS or AUDMAIN_STATUS_FAILURE.

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audqcp_evb_extract_file_header (
  uint8 *buffer,
  uint32 *index
) {
  if ( memcmp(evb_id, buffer, AUDEVB_HEADER_SIZE) == 0 ) {
    *index = *index + AUDEVB_HEADER_SIZE;
    return AUDMAIN_STATUS_SUCCESS;
  }
  else {
    return AUDMAIN_STATUS_FAILURE;
  }
}
#endif /* FEATURE_AUDFMT_EVB */


/* <EJECT> */
#ifdef FEATURE_AUDFMT_EVW
/*===========================================================================

FUNCTION audqcp_evw_extract_file_header

DESCRIPTION
  This function extracts the header of the EVRC-WB file.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_SUCCESS or AUDMAIN_STATUS_FAILURE.

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audqcp_evw_extract_file_header (
  uint8 *buffer,
  uint32 *index
) {
  if (memcmp(evw_id, buffer, AUDEVW_HEADER_SIZE) == 0 )
  {
    *index = *index + AUDEVW_HEADER_SIZE;
    audqcp_parse_ctl.format = QCPSUP_EVW_FILE_FORMAT;
    return AUDMAIN_STATUS_SUCCESS;
  } else {
    return AUDMAIN_STATUS_FAILURE;
  }
}
#endif /* FEATURE_AUDFMT_EVW */


/* <EJECT> */
/*===========================================================================

FUNCTION audqcp_count_frames

DESCRIPTION
  This function counts the number of valid QCP frames in the buffer.

DEPENDENCIES
  None

RETURN VALUE
  Number of valid frames.
  AUDMAIN_STATUS_SUCCESS - if the whole buffer is composed of valid frames.
  AUDMAIN_STATUS_ERROR   - if any invalid frames are encountered in the length
                           of the buffer.

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audqcp_count_frames (
  qcpsup_format_enum_type  format,
  uint8                    *buffer,
  uint32                   *index,
  uint32                   length,
  uint16                   *num_frames
) {
  boolean status = TRUE;
  uint32  cur_index, frame_size=0;
  uint16  frames;

  frames    = 0;
  cur_index = *index;

  if( cur_index > length )
  {
    MSG_ERROR("cur_index if OOB error",0,0,0);
	return(AUDMAIN_STATUS_ERROR);
  }
  
  while((cur_index < length) &&
    (status = qcpsup_validate_frame(format, buffer[cur_index], &frame_size)) &&
    ((cur_index + frame_size + 1) <= length)) {
    cur_index += frame_size + 1;
    frames ++;
  }

  /* Validate last frame size against total file size. Last frame size can be junk
  value which lead integer overflow in cur_index. No need to check cur_index
  against length here */
  if( frame_size > length )
  {
    MSG_ERROR("Last frame size is invalid",0,0,0);
	return(AUDMAIN_STATUS_ERROR);
  }

  if(status) {
    *index      = cur_index;
    *num_frames = frames;
    return(AUDMAIN_STATUS_SUCCESS);
  } else {
    MSG_ERROR("Failed on frame %d at pos %d",frames, cur_index,0);
    return(AUDMAIN_STATUS_ERROR);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION audqcp_func_get_data_cb_req

DESCRIPTION
  This function handles the get data callback for QCP function data
  requests.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean audqcp_func_get_data_cb_req (
  uint8 **data,
  uint8 *buf,
  uint32 length,
  uint32 pos
) {
  snd_af_access_type *handle;
  boolean            ret_val = FALSE;

  if(data == &audqcp_func_buffer) {
    *data = buf;

    if(audqcp_func_handle != NULL) {
      handle = audqcp_func_handle;
      audqcp_func_handle = NULL;
      audqcp_func_req(handle, buf, pos, length);
    } else if(audqcp_get_chunk_handle != NULL) {
      if(audqcp_get_chunk_length != 0) {
        if((audqcp_get_chunk_handle != NULL) && 
                         (audqcp_get_chunk_handle->data_func != NULL)) {
          audqcp_get_chunk_handle->data_func(SND_CMX_QCP_SUCCESS,
                     audqcp_get_chunk_handle->audfmt_ptr,
                     audqcp_get_chunk_length, buf);
        }
        audqcp_get_chunk_handle = NULL;
        audqcp_get_chunk_length = 0;
        audmain_reset_qcp_func();
      } else {
        handle = audqcp_get_chunk_handle;
        audqcp_get_chunk_handle = NULL;
        audqcp_chunk_req(handle, buf, pos, length);
      }
    }
    ret_val = TRUE;
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audqcp_get_data_cb_req

DESCRIPTION
  This function handles the get data callback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  audqcp_parse_ctl
  audqcp_buf_size

===========================================================================*/
boolean audqcp_get_data_cb_req (
  uint8 **data,
  uint8 *buf,
  uint32 length,
  uint32 pos
) {
  audqcp_prompt_type    *cur_prompt, *prev_prompt;
  boolean               enqueue_first, ret_val = FALSE;
  uint8                 *cur_buf, *next_buf;
  uint32                index, tlength;

  if(audqcp_buf_mgmt.skip != NULL) {
    audmain_do_play_cb(audqcp_parse_ctl.audqcp_handle, SND_CMX_AF_FREE_BUF, 0,
                       (void *) buf);
    audqcp_buf_mgmt.skip = NULL;

    if (audqcp_parse_ctl.status_flag & AUDQCP_MASK_RESTART) {
      if(audqcp_buf_mgmt.first != NULL) {
        (void) audmain_get_data(audqcp_parse_ctl.audqcp_handle,
                                audqcp_parse_ctl.file_location,
                                &audqcp_buf_mgmt.first->track_buf);
      } else {
        MSG_ERROR("restart playback failed, null pointer",0,0,0);
      }
    }
    return (TRUE);
  }

  if (buf == NULL ) {
    audqcp_parse_ctl.status_flag |= AUDQCP_MASK_FAILURE;
    audqcp_parse_ctl.error_status = AUDMAIN_STATUS_DATA_ACCESS_ERR;
    audmain_do_play_cb(audqcp_parse_ctl.audqcp_handle, SND_CMX_AF_FREE_BUF, 0,
                       (void *) buf);
    return (TRUE);
  }

  if(audqcp_buf_mgmt.first != NULL) {
    if(audqcp_buf_mgmt.first == &audqcp_parse_ctl.prompt1) {
      prev_prompt = &audqcp_parse_ctl.prompt2;
    } else if(audqcp_buf_mgmt.first == &audqcp_parse_ctl.prompt2) {
      prev_prompt = &audqcp_parse_ctl.prompt1;
    } else {
      MSG_HIGH("Invalid data callback.",0,0,0);
      return(FALSE);
    }

    cur_prompt = audqcp_buf_mgmt.first;

    cur_prompt->start_frame = prev_prompt->start_frame +
                              prev_prompt->boarder.pb_num_frames +
                              prev_prompt->prompt.pb_num_frames;

    *data = buf;

    if((audqcp_buf_mgmt.cmd_buf->prompt1 == NULL) && 
       (audqcp_buf_mgmt.cmd_buf->prompt2 == NULL)) {
      enqueue_first = TRUE;
    } else {
      enqueue_first = FALSE;
    }

    if(audqcp_parse_ctl.remain != 0) {
      cur_buf = audqcp_parse_ctl.frame[audqcp_parse_ctl.frame_index];
      audqcp_parse_ctl.frame_index++;
      if(audqcp_parse_ctl.frame_index == 3) {
        audqcp_parse_ctl.frame_index = 0;
      }

      memcpy(&cur_buf[audqcp_parse_ctl.index], buf, audqcp_parse_ctl.remain);

      cur_prompt->boarder.pb_data_ptr         = cur_buf;
      cur_prompt->boarder.pb_num_frames       = 1;
#ifdef FEATURE_AUDFMT_AMR
      if(audqcp_parse_ctl.format == QCPSUP_AMR_FILE_FORMAT) {
        cur_prompt->boarder.pb_voc_capability = VOC_CAP_AMR;
        cur_prompt->boarder.pb_len_rate_var   = VOC_PB_AMR;
      }
#endif /* FEATURE_AUDFMT_AMR */

      if(audqcp_parse_ctl.format == QCPSUP_AWB_FILE_FORMAT) {
        cur_prompt->boarder.pb_voc_capability = VOC_CAP_AMR_WB;
        cur_prompt->boarder.pb_len_rate_var   = VOC_PB_AWB;
      }

#ifdef FEATURE_AUDFMT_EVB
      if(audqcp_parse_ctl.format == QCPSUP_EVB_FILE_FORMAT) {
        cur_prompt->boarder.pb_voc_capability = VOC_CAP_4GV_NB;
        cur_prompt->boarder.pb_len_rate_var   = VOC_PB_EVB;
      }
#endif /* FEATURE_AUDFMT_EVB */

#ifdef FEATURE_AUDFMT_EVW
      if(audqcp_parse_ctl.format == QCPSUP_EVW_FILE_FORMAT) {
        cur_prompt->boarder.pb_voc_capability = VOC_CAP_4GV_WB;
        cur_prompt->boarder.pb_len_rate_var   = VOC_PB_EVW;
      }
#endif /* FEATURE_AUDFMT_EVW */

      if(audqcp_parse_ctl.format == QCPSUP_FORMAT_13K) {
        cur_prompt->boarder.pb_voc_capability = VOC_CAP_IS733;
        cur_prompt->boarder.pb_len_rate_var   = VOC_PB_NATIVE_QCP;
      }
      if(audqcp_parse_ctl.format == QCPSUP_FORMAT_EVRC) {
        cur_prompt->boarder.pb_voc_capability = VOC_CAP_IS127;
        cur_prompt->boarder.pb_len_rate_var   = VOC_PB_NATIVE_QCP;
      }
      if(audqcp_parse_ctl.format == QCPSUP_FORMAT_4GV_NB) {
        cur_prompt->boarder.pb_voc_capability = VOC_CAP_4GV_NB;
        cur_prompt->boarder.pb_len_rate_var   = VOC_PB_NATIVE_QCP;
      }
      if(audqcp_parse_ctl.format == QCPSUP_FORMAT_AMR) {
        cur_prompt->boarder.pb_voc_capability = VOC_CAP_AMR;
        cur_prompt->boarder.pb_len_rate_var   = VOC_PB_NATIVE_QCP;
      }
      if(audqcp_parse_ctl.format == QCPSUP_FORMAT_EFR) {
        cur_prompt->boarder.pb_voc_capability = VOC_CAP_GSM_EFR;
        cur_prompt->boarder.pb_len_rate_var   = VOC_PB_NATIVE_QCP;
      }
      if(audqcp_parse_ctl.format == QCPSUP_FORMAT_FR) {
        cur_prompt->boarder.pb_voc_capability = VOC_CAP_GSM_FR;
        cur_prompt->boarder.pb_len_rate_var   = VOC_PB_NATIVE_QCP;
      }
      if(audqcp_parse_ctl.format == QCPSUP_FORMAT_HR) {
        cur_prompt->boarder.pb_voc_capability = VOC_CAP_GSM_HR;
        cur_prompt->boarder.pb_len_rate_var   = VOC_PB_NATIVE_QCP;
      }
      cur_prompt->boarder.pb_direction        = audqcp_parse_ctl.direction;
      cur_prompt->boarder.in_call             = audqcp_parse_ctl.in_call;

      if(enqueue_first) {
        audqcp_buf_mgmt.cmd_buf->prompt1 = &(cur_prompt->boarder);
      } else {
        audqcp_buf_mgmt.cmd_buf->prompt3 = &(cur_prompt->boarder);
      }
    } else {
      cur_prompt->boarder.pb_num_frames = 0;
    }

    index                          = audqcp_parse_ctl.remain;
    cur_prompt->prompt.pb_data_ptr = &buf[index];

    if(audqcp_parse_ctl.track_length == AUDQCP_MAX_32BIT) {
      tlength = length;
    } else {
      tlength = MIN(length, audqcp_parse_ctl.track_start +
                audqcp_parse_ctl.track_length - audqcp_parse_ctl.file_location);
    }

    /* Check format, get frame count */
    if(audqcp_count_frames(audqcp_parse_ctl.format, buf, &index, tlength,
                           &cur_prompt->prompt.pb_num_frames) ==
       AUDMAIN_STATUS_ERROR) {
      audqcp_parse_ctl.status_flag |= AUDQCP_MASK_FAILURE;
      audqcp_parse_ctl.error_status = AUDMAIN_STATUS_DATA_ERR;
      MSG_ERROR("Invalid QCP data.",0,0,0);
      /* The buffere returned from upper lower is valid, but the data
         is corrupted. So, still return true */
      return(TRUE); 
    }

    if(cur_prompt->prompt.pb_num_frames != 0) {
#ifdef FEATURE_AUDFMT_AMR
      if(audqcp_parse_ctl.format == QCPSUP_AMR_FILE_FORMAT) {
        cur_prompt->prompt.pb_voc_capability  = VOC_CAP_AMR;
        cur_prompt->prompt.pb_len_rate_var    = VOC_PB_AMR;
      }
#endif /* FEATURE_AUDFMT_AMR */

      if(audqcp_parse_ctl.format == QCPSUP_AWB_FILE_FORMAT) {
        cur_prompt->prompt.pb_voc_capability  = VOC_CAP_AMR_WB;
        cur_prompt->prompt.pb_len_rate_var    = VOC_PB_AWB;
      }

#ifdef FEATURE_AUDFMT_EVB
      if(audqcp_parse_ctl.format == QCPSUP_EVB_FILE_FORMAT) {
        cur_prompt->prompt.pb_voc_capability  = VOC_CAP_4GV_NB;
        cur_prompt->prompt.pb_len_rate_var    = VOC_PB_EVB;
      }
#endif /* FEATURE_AUDFMT_EVB */

#ifdef FEATURE_AUDFMT_EVW
      if(audqcp_parse_ctl.format == QCPSUP_EVW_FILE_FORMAT) {
        cur_prompt->prompt.pb_voc_capability  = VOC_CAP_4GV_WB;
        cur_prompt->prompt.pb_len_rate_var    = VOC_PB_EVW;
      }
#endif /* FEATURE_AUDFMT_EVW */

      if(audqcp_parse_ctl.format == QCPSUP_FORMAT_13K) {
        cur_prompt->prompt.pb_voc_capability  = VOC_CAP_IS733;
        cur_prompt->prompt.pb_len_rate_var    = VOC_PB_NATIVE_QCP;
      }
      if(audqcp_parse_ctl.format == QCPSUP_FORMAT_EVRC) {
        cur_prompt->prompt.pb_voc_capability  = VOC_CAP_IS127;
        cur_prompt->prompt.pb_len_rate_var    = VOC_PB_NATIVE_QCP;
      }
      if(audqcp_parse_ctl.format == QCPSUP_FORMAT_4GV_NB) {
        cur_prompt->prompt.pb_voc_capability  = VOC_CAP_4GV_NB;
        cur_prompt->prompt.pb_len_rate_var    = VOC_PB_NATIVE_QCP;
      }
      if (audqcp_parse_ctl.format == QCPSUP_FORMAT_AMR) {
        cur_prompt->prompt.pb_voc_capability  = VOC_CAP_AMR;
        cur_prompt->prompt.pb_len_rate_var    = VOC_PB_NATIVE_QCP;
      }
      if (audqcp_parse_ctl.format == QCPSUP_FORMAT_EFR) {
        cur_prompt->prompt.pb_voc_capability  = VOC_CAP_GSM_EFR;
        cur_prompt->prompt.pb_len_rate_var    = VOC_PB_NATIVE_QCP;
      }
      if (audqcp_parse_ctl.format == QCPSUP_FORMAT_FR) {
        cur_prompt->prompt.pb_voc_capability  = VOC_CAP_GSM_FR;
        cur_prompt->prompt.pb_len_rate_var    = VOC_PB_NATIVE_QCP;
      }
      if (audqcp_parse_ctl.format == QCPSUP_FORMAT_HR) {
        cur_prompt->prompt.pb_voc_capability  = VOC_CAP_GSM_HR;
        cur_prompt->prompt.pb_len_rate_var    = VOC_PB_NATIVE_QCP;
      }
      cur_prompt->prompt.pb_direction         = audqcp_parse_ctl.direction;
      cur_prompt->prompt.in_call              = audqcp_parse_ctl.in_call;

      if(enqueue_first) {
        audqcp_buf_mgmt.cmd_buf->prompt2      = &(cur_prompt->prompt);
      } else {
        audqcp_buf_mgmt.cmd_buf->prompt4      = &(cur_prompt->prompt);
      }
    } else {
      audmain_do_play_cb(audqcp_parse_ctl.audqcp_handle, SND_CMX_AF_FREE_BUF,
                         0, (void *) buf);
      cur_prompt->track_buf = NULL;
    }

    audqcp_parse_ctl.file_location += tlength;

    if (tlength == 0) {
      audqcp_parse_ctl.index = 0;
    } else if ((int32) (tlength - index) >= 0 ){
      audqcp_parse_ctl.index = tlength - index;
    } else {  
	  audqcp_parse_ctl.status_flag |= AUDQCP_MASK_FAILURE;  
	  audqcp_parse_ctl.error_status = AUDMAIN_STATUS_DATA_ERR;     
	  return(TRUE); 
    }

    /* If any data is left over after checking valid frames */
    if(audqcp_parse_ctl.index != 0) {
      next_buf = audqcp_parse_ctl.frame[audqcp_parse_ctl.frame_index];
      memcpy(next_buf, &buf[index], audqcp_parse_ctl.index);

      if(qcpsup_validate_frame(audqcp_parse_ctl.format, buf[index], &index)) {
        audqcp_parse_ctl.remain = index - audqcp_parse_ctl.index + 1;
      } else {
        MSG_ERROR("Invalid frame header: %d",buf[index],0,0);
        return(FALSE);
      }
    } else {
      audqcp_parse_ctl.remain = 0;
    }

    audqcp_parse_ctl.status_flag &= ~AUDQCP_MASK_RESTART;

    audqcp_buf_mgmt.first  = audqcp_buf_mgmt.second;
    audqcp_buf_mgmt.second = NULL;

    if(audqcp_buf_mgmt.first != NULL) {
      if((audqcp_parse_ctl.track_length == AUDQCP_MAX_32BIT) ||
         ((audqcp_parse_ctl.file_location - audqcp_parse_ctl.track_start) <
           audqcp_parse_ctl.track_length)) {

        (void) audmain_get_data(audqcp_parse_ctl.audqcp_handle,
                                audqcp_parse_ctl.file_location,
                                &audqcp_buf_mgmt.first->track_buf);
      } else {
        audqcp_buf_mgmt.first = NULL;
      }
    }

    ret_val = TRUE;
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audqcp_get_data

DESCRIPTION
  This function tries to retrieve the next data buffer.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_SUCCESS - if things have been set up to get data
  AUDMAIN_STATUS_DONE    - if already at the end of file
  AUDMAIN_STATUS_FAILURE - if error occurs

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audqcp_get_data (
  audqcp_parse_ctl_type   *qcp_ctl,
  audmain_cmd_buf_type    *cmd_buf,
  audqcp_prompt_type      *prompt
) {
  audmain_status_type ret_val = AUDMAIN_STATUS_SUCCESS;

  if((qcp_ctl != NULL) && (cmd_buf != NULL)) {
    if((prompt != NULL) && (prompt->track_buf != NULL)) {
      audmain_do_play_cb(qcp_ctl->audqcp_handle, SND_CMX_AF_FREE_BUF, 0,
                         (void *) prompt->track_buf);
      prompt->track_buf = NULL;

      if((qcp_ctl->track_length == AUDQCP_MAX_32BIT) ||
         ((qcp_ctl->file_location - qcp_ctl->track_start) <
           qcp_ctl->track_length)) {
        if(audqcp_buf_mgmt.first == NULL) {
          audqcp_buf_mgmt.first = prompt;

          audqcp_buf_mgmt.cmd_buf = cmd_buf;
          (void) audmain_get_data(qcp_ctl->audqcp_handle,
                                  qcp_ctl->file_location, &prompt->track_buf);

        } else if(audqcp_buf_mgmt.second == NULL) {
          audqcp_buf_mgmt.second = prompt;
          ret_val                = AUDMAIN_STATUS_BUFF_ERR;
        } else {
          MSG_ERROR("Should not get here",0,0,0);
          ret_val = AUDMAIN_STATUS_FAILURE;
        }
      } else {
        ret_val = AUDMAIN_STATUS_DONE;
      }
    } else if((qcp_ctl->file_location - qcp_ctl->track_start) >=
              qcp_ctl->track_length) {
      ret_val = AUDMAIN_STATUS_DONE;
    }
  } else {
    MSG_ERROR("Null buffer pointer", 0,0,0);
    ret_val = AUDMAIN_STATUS_FAILURE;
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audqcp_format_play

DESCRIPTION
  This function sets up to play a QCP file with no header information.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  audqcp_parse_ctl
  audqcp_buf_size

===========================================================================*/
audmain_status_type audqcp_format_play (
  snd_cmx_format_spec_type *input_spec,
  uint8                    *buffer,
  snd_af_access_type       *handle,
  audmain_parse_ctl_type   *parse_ctl
) {
  uint32 index = 0;

  if(buffer == NULL) {
    MSG_ERROR("Null buffer for header.",0,0,0);
    return(AUDMAIN_STATUS_ERROR);
  } else {
    audqcp_buf_mgmt.skip   = NULL;
    audqcp_buf_mgmt.first  = NULL;
    audqcp_buf_mgmt.second = NULL;
    audqcp_buf_size        = parse_ctl->buffer_size;

    audqcp_parse_ctl.track_length = AUDQCP_MAX_32BIT;
    audqcp_parse_ctl.format = QCPSUP_FORMAT_INVALID;
#ifdef FEATURE_AUDFMT_AMR
    if(input_spec->codec_type.file_type == SND_CMX_AF_FILE_AMR) {
      audqcp_parse_ctl.format       = QCPSUP_AMR_FILE_FORMAT;
    }
#endif /* FEATURE_AUDFMT_AMR */

#ifdef FEATURE_AUDFMT_EVB
    if(input_spec->codec_type.file_type == SND_CMX_AF_FILE_EVB) {
      audqcp_parse_ctl.format       = QCPSUP_EVB_FILE_FORMAT;
    }
#endif /* FEATURE_AUDFMT_EVB */

#ifdef FEATURE_AUDFMT_EVW
    if(input_spec->codec_type.file_type == SND_CMX_AF_FILE_EVW) {
      audqcp_parse_ctl.format       = QCPSUP_EVW_FILE_FORMAT;
    }
#endif /* FEATURE_AUDFMT_EVW */

    if(input_spec->codec_type.file_type == SND_CMX_AF_FILE_QCP_13K) {
      audqcp_parse_ctl.format       = QCPSUP_FORMAT_13K;
    }
    if(input_spec->codec_type.file_type == SND_CMX_AF_FILE_QCP_EVRC) {
      audqcp_parse_ctl.format       = QCPSUP_FORMAT_EVRC;
    }
    if(input_spec->codec_type.file_type == SND_CMX_AF_FILE_QCP_4GV_NB) {
      audqcp_parse_ctl.format       = QCPSUP_FORMAT_4GV_NB;
    }
    if(input_spec->codec_type.file_type == SND_CMX_AF_FILE_QCP_AMR) {
      audqcp_parse_ctl.format       = QCPSUP_FORMAT_AMR;
    }
    if(input_spec->codec_type.file_type == SND_CMX_AF_FILE_QCP_EFR) {
      audqcp_parse_ctl.format       = QCPSUP_FORMAT_EFR;
    }
    if(input_spec->codec_type.file_type == SND_CMX_AF_FILE_QCP_FR) {
      audqcp_parse_ctl.format       = QCPSUP_FORMAT_FR;
    }
    if(input_spec->codec_type.file_type == SND_CMX_AF_FILE_QCP_HR) {
      audqcp_parse_ctl.format       = QCPSUP_FORMAT_HR;
    }

    /* Sanity check that the file is supported */
    if(audqcp_parse_ctl.format == QCPSUP_FORMAT_INVALID) {
      return(AUDMAIN_STATUS_ERROR);
    }

    /* Initialize values */
    audqcp_parse_ctl.prompt1.prompt.pb_num_frames  = 0;
    audqcp_parse_ctl.prompt1.boarder.pb_num_frames = 0;
    audqcp_parse_ctl.prompt1.start_frame           = 0;
    audqcp_parse_ctl.prompt2.prompt.pb_num_frames  = 0;
    audqcp_parse_ctl.prompt2.boarder.pb_num_frames = 0;
    audqcp_parse_ctl.prompt2.start_frame           = 0;
    audqcp_parse_ctl.seek_ms                       = 0;

    audqcp_parse_ctl.prompt1.track_buf = buffer;
    audqcp_parse_ctl.audqcp_handle     = handle;
    audqcp_parse_ctl.direction         = VOC_PB_DIR_FORWARD;

    audqcp_parse_ctl.track_start       = 0;
    audqcp_parse_ctl.prompt1.prompt.pb_data_ptr =
                                 audqcp_parse_ctl.prompt1.track_buf;

    if(audqcp_count_frames(audqcp_parse_ctl.format, buffer,
                           &index, audqcp_buf_size,
                           &audqcp_parse_ctl.prompt1.prompt.pb_num_frames) ==
       AUDMAIN_STATUS_ERROR) {
      MSG_ERROR("Invalid QCP data.",0,0,0);
      return(AUDMAIN_STATUS_ERROR);
    }

#ifdef FEATURE_AUDFMT_AMR
    if(audqcp_parse_ctl.format == QCPSUP_AMR_FILE_FORMAT) {
      audqcp_parse_ctl.prompt1.prompt.pb_voc_capability = VOC_CAP_AMR;
      audqcp_parse_ctl.prompt1.prompt.pb_len_rate_var   = VOC_PB_AMR;
    }
#endif /* FEATURE_AUDFMT_AMR */

    if(audqcp_parse_ctl.format == QCPSUP_AWB_FILE_FORMAT) {
      audqcp_parse_ctl.prompt1.prompt.pb_voc_capability = VOC_CAP_AMR_WB;
      audqcp_parse_ctl.prompt1.prompt.pb_len_rate_var   = VOC_PB_AWB;
    }

#ifdef FEATURE_AUDFMT_EVB
    if(audqcp_parse_ctl.format == QCPSUP_EVB_FILE_FORMAT) {
      audqcp_parse_ctl.prompt1.prompt.pb_voc_capability = VOC_CAP_4GV_NB;
      audqcp_parse_ctl.prompt1.prompt.pb_len_rate_var   = VOC_PB_EVB;
    }
#endif /* FEATURE_AUDFMT_EVB */

#ifdef FEATURE_AUDFMT_EVW
    if(audqcp_parse_ctl.format == QCPSUP_EVW_FILE_FORMAT) {
      audqcp_parse_ctl.prompt1.prompt.pb_voc_capability = VOC_CAP_4GV_WB;
      audqcp_parse_ctl.prompt1.prompt.pb_len_rate_var   = VOC_PB_EVW;
    }
#endif /* FEATURE_AUDFMT_EVW */

    if(audqcp_parse_ctl.format == QCPSUP_FORMAT_13K) {
      audqcp_parse_ctl.prompt1.prompt.pb_voc_capability = VOC_CAP_IS733;
      audqcp_parse_ctl.prompt1.prompt.pb_len_rate_var   = VOC_PB_NATIVE_QCP;
    }
    if(audqcp_parse_ctl.format == QCPSUP_FORMAT_EVRC) {
      audqcp_parse_ctl.prompt1.prompt.pb_voc_capability = VOC_CAP_IS127;
      audqcp_parse_ctl.prompt1.prompt.pb_len_rate_var   = VOC_PB_NATIVE_QCP;
    }
    if(audqcp_parse_ctl.format == QCPSUP_FORMAT_4GV_NB) {
      audqcp_parse_ctl.prompt1.prompt.pb_voc_capability = VOC_CAP_4GV_NB;
      audqcp_parse_ctl.prompt1.prompt.pb_len_rate_var   = VOC_PB_NATIVE_QCP;
    }
    if(audqcp_parse_ctl.format == QCPSUP_FORMAT_AMR) {
      audqcp_parse_ctl.prompt1.prompt.pb_voc_capability = VOC_CAP_AMR;
      audqcp_parse_ctl.prompt1.prompt.pb_len_rate_var   = VOC_PB_NATIVE_QCP;
    }
    if(audqcp_parse_ctl.format == QCPSUP_FORMAT_EFR) {
      audqcp_parse_ctl.prompt1.prompt.pb_voc_capability = VOC_CAP_GSM_EFR;
      audqcp_parse_ctl.prompt1.prompt.pb_len_rate_var   = VOC_PB_NATIVE_QCP;
    }
    if(audqcp_parse_ctl.format == QCPSUP_FORMAT_FR) {
      audqcp_parse_ctl.prompt1.prompt.pb_voc_capability = VOC_CAP_GSM_FR;
      audqcp_parse_ctl.prompt1.prompt.pb_len_rate_var   = VOC_PB_NATIVE_QCP;
    }
    if(audqcp_parse_ctl.format == QCPSUP_FORMAT_HR) {
      audqcp_parse_ctl.prompt1.prompt.pb_voc_capability = VOC_CAP_GSM_HR;
      audqcp_parse_ctl.prompt1.prompt.pb_len_rate_var   = VOC_PB_NATIVE_QCP;
    }

    audqcp_parse_ctl.prompt1.prompt.pb_direction = audqcp_parse_ctl.direction;
    audqcp_parse_ctl.prompt1.prompt.in_call      = audqcp_parse_ctl.in_call;
    parse_ctl->current_buf.prompt1 = &(audqcp_parse_ctl.prompt1.prompt);

    audqcp_parse_ctl.file_location     = audqcp_buf_size;
    parse_ctl->current_buf.prompt2     = NULL;
    audqcp_parse_ctl.prompt2.track_buf = NULL;

    audqcp_parse_ctl.frame_index = 0;
    audqcp_parse_ctl.index       = audqcp_buf_size - index;
    audqcp_parse_ctl.status_flag = 0;

    if(audqcp_parse_ctl.index != 0) {
      memcpy(audqcp_parse_ctl.frame[0],
             &audqcp_parse_ctl.prompt1.track_buf[index],
             audqcp_parse_ctl.index);

      if(qcpsup_validate_frame(audqcp_parse_ctl.format,
                               audqcp_parse_ctl.frame[0][0], &index)) {
        audqcp_parse_ctl.remain = index - audqcp_parse_ctl.index + 1;
      } else {
        return(AUDMAIN_STATUS_ERROR);
      }
    } else {
      audqcp_parse_ctl.remain = 0;
    }

    audmain_set_read_sync(FALSE);
    if(audmain_get_data(handle, audqcp_parse_ctl.file_location,
        &(audqcp_parse_ctl.prompt2.track_buf)) != AUDMAIN_STATUS_SUCCESS) {
      return(AUDMAIN_STATUS_FAILURE);
    } else {
      audqcp_buf_mgmt.first   = &audqcp_parse_ctl.prompt2;
      audqcp_buf_mgmt.cmd_buf = &parse_ctl->current_buf;
    }
  }

  parse_ctl->tracks = &audqcp_parse_ctl;
  return(AUDMAIN_STATUS_SUCCESS);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audqcp_do_file_spec

DESCRIPTION
  This function builds a sound file specifications buffer and transmits the
  data to the CMX layer through the playback callback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void audqcp_do_file_spec (
  snd_af_access_type    *handle,
  audqcp_parse_ctl_type *qcp_ctl
)
{
  snd_cmx_af_spec_type snd_spec;

  switch (qcp_ctl->format) {
    case QCPSUP_FORMAT_13K:
      snd_spec.file_type = SND_CMX_AF_FILE_QCP_13K;
      break;

    case QCPSUP_FORMAT_EVRC:
      snd_spec.file_type = SND_CMX_AF_FILE_QCP_EVRC;
      break;

    case QCPSUP_FORMAT_4GV_NB:
      snd_spec.file_type = SND_CMX_AF_FILE_QCP_4GV_NB;
      break;

    case QCPSUP_FORMAT_AMR:
      snd_spec.file_type = SND_CMX_AF_FILE_QCP_AMR;
      break;

    case QCPSUP_FORMAT_EFR:
      snd_spec.file_type = SND_CMX_AF_FILE_QCP_EFR;
      break;

    case QCPSUP_FORMAT_FR:
      snd_spec.file_type = SND_CMX_AF_FILE_QCP_FR;
      break;

    case QCPSUP_FORMAT_HR:
      snd_spec.file_type = SND_CMX_AF_FILE_QCP_HR;
      break;

    case QCPSUP_AMR_FILE_FORMAT:
      snd_spec.file_type = SND_CMX_AF_FILE_AMR;
      break;

    case QCPSUP_AWB_FILE_FORMAT:
      snd_spec.file_type = SND_CMX_AF_FILE_AWB;
      break;

#ifdef FEATURE_AUDFMT_EVB
    case QCPSUP_EVB_FILE_FORMAT:
      snd_spec.file_type = SND_CMX_AF_FILE_EVB;
      break;
#endif /* FEATURE_AUDFMT_EVB */


#ifdef FEATURE_AUDFMT_EVW
    case QCPSUP_EVW_FILE_FORMAT:
      snd_spec.file_type = SND_CMX_AF_FILE_EVW;
      break;
#endif /* FEATURE_AUDFMT_EVW */

    case QCPSUP_FORMAT_INVALID:
    default:
      snd_spec.file_type = SND_CMX_AF_FILE_UNKNOWN;
      break;
  }
  snd_spec.qcp_spec.supported_ops = SND_SUPPORTED_OPS_MASK_SEEK |
                                    SND_SUPPORTED_OPS_MASK_GETTIME;
  audmain_do_play_cb( handle, SND_CMX_AF_AUDIO_SPEC, 0, (void *)&snd_spec );
}
  

/* <EJECT> */
/*===========================================================================

FUNCTION audqcp_parse_head

DESCRIPTION
  This function parses and validates the head of a QCP file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  audqcp_parse_ctl
  audqcp_buf_size

===========================================================================*/
audmain_status_type audqcp_parse_head (
  uint8                       *buffer,
  snd_af_access_type          *handle,
  snd_audfmt_start_enum_type  start_position,
  audmain_parse_ctl_type      *parse_ctl
) {
  uint32 index = 0, index2 = 0;
  uint32 file_size, buffer_data_size, buffer_start = 0;
  uint8  riff_id[]  = {'Q', 'L', 'C', 'M'};
  uint8  chunk_id[] = {'d', 'a', 't', 'a'};
  qcpsup_status_type status;
  audmain_status_type ret_val = AUDMAIN_STATUS_FAILURE;
  boolean vrat_found = TRUE;

  if(buffer == NULL) {
    MSG_ERROR("Null buffer for header.",0,0,0);
    return(AUDMAIN_STATUS_ERROR);
  } else {
    audqcp_buf_mgmt.skip   = NULL;
    audqcp_buf_mgmt.first  = NULL;
    audqcp_buf_mgmt.second = NULL;
    audqcp_buf_size        = parse_ctl->buffer_size;

    audqcp_parse_ctl.track_length = 0xffff;

    audqcp_parse_ctl.num_frames = 0;

#ifdef FEATURE_AUDFMT_AMR
    /* Check if AMR file */
    ret_val = audqcp_amr_extract_file_header(buffer, &index, &audqcp_parse_ctl);
  
    if(ret_val == AUDMAIN_STATUS_SUCCESS) {
      audqcp_parse_ctl.format = QCPSUP_AMR_FILE_FORMAT;
      audmain_do_play_cb(handle, SND_CMX_AF_GET_FILE_LEN, 0,
                                       (void *)&audqcp_parse_ctl.track_length);
      audqcp_parse_ctl.track_length -= index;
    } else if(ret_val == AUDMAIN_STATUS_ERROR) {
      /* AMR file, but unsupported mode */
      return(AUDMAIN_STATUS_ERROR);
    } else
#endif /* FEATURE_AUDFMT_AMR */
    /* check if .awb file */
    if ((ret_val = audqcp_amr_wb_extract_file_header(buffer, &index,\
                &audqcp_parse_ctl)) == AUDMAIN_STATUS_SUCCESS) {
      /* setting the file format as .awb */
      MSG_HIGH("single channel .awb file found",0, 0, 0);
      audqcp_parse_ctl.format = QCPSUP_AWB_FILE_FORMAT;
      audmain_do_play_cb(handle, SND_CMX_AF_GET_FILE_LEN, 0,
                                       (void *)&audqcp_parse_ctl.track_length);
      audqcp_parse_ctl.track_length -= index;
    } else if(ret_val == AUDMAIN_STATUS_ERROR) {
      MSG_HIGH("multi channels .awb file found but it's not supported",0, 0, 0);
      /* .AWB file, but unsupported mode */
      return(AUDMAIN_STATUS_ERROR);
    } else
#ifdef FEATURE_AUDFMT_EVB
    /* Check if EVRC-B file */
    if ( audqcp_evb_extract_file_header(buffer, &index) ==
                    AUDMAIN_STATUS_SUCCESS ) {
      audqcp_parse_ctl.format = QCPSUP_EVB_FILE_FORMAT;
      audmain_do_play_cb(handle, SND_CMX_AF_GET_FILE_LEN, 0,
                                       (void *)&audqcp_parse_ctl.track_length);
      audqcp_parse_ctl.track_length -= index;
    } else
#endif /* FEATURE_AUDFMT_EVB */
#ifdef FEATURE_AUDFMT_EVW
    /* Check if EVRC-B file */
    if ( audqcp_evw_extract_file_header(buffer, &index) ==
                    AUDMAIN_STATUS_SUCCESS ) {
      audqcp_parse_ctl.format = QCPSUP_EVW_FILE_FORMAT;
      audmain_do_play_cb(handle, SND_CMX_AF_GET_FILE_LEN, 0,
                                       (void *)&audqcp_parse_ctl.track_length);
      audqcp_parse_ctl.track_length -= index;
    } else
#endif /* FEATURE_AUDFMT_EVW */
    if(qcpsup_do_riff(riff_id, buffer, &index, &file_size) ==
       QCPSUP_STATUS_SUCCESS) {
      while((status = qcpsup_get_fmt(buffer, &index, audqcp_buf_size,
                                     &audqcp_parse_ctl.format)) == 
            QCPSUP_STATUS_BUFF_ERROR)
      {
        if((index + buffer_start) > file_size) {
          return(AUDMAIN_STATUS_ERROR);
        }
        audmain_do_play_cb(handle, SND_CMX_AF_FREE_BUF, 0, (void *) buffer);

        if(audmain_get_data(handle, buffer_start + index, &buffer) !=
           AUDMAIN_STATUS_SUCCESS) {
          return(AUDMAIN_STATUS_ERROR);
        } else {
          buffer_start  += index;
          index          = 0;
        }
      }

      /* Added for # of frames for time calculation */
      if(status == QCPSUP_STATUS_SUCCESS) {
        index2 = index;
        while((status = qcpsup_get_vrat(buffer, &index2, audqcp_buf_size,
                                        &audqcp_parse_ctl.num_frames)) == 
              QCPSUP_STATUS_BUFF_ERROR && vrat_found == TRUE) {

          if( qcpsup_find_chunk(chunk_id, buffer, &index, audqcp_buf_size,
               &audqcp_parse_ctl.track_length) == QCPSUP_STATUS_SUCCESS) {
            vrat_found = FALSE;
            status = QCPSUP_STATUS_SUCCESS;
            break;
          }
 
          if((index + buffer_start) > file_size) {
            return(AUDMAIN_STATUS_ERROR);
          }

          audmain_do_play_cb(handle, SND_CMX_AF_FREE_BUF, 0, (void *) buffer);
    
          if(audmain_get_data(handle, buffer_start + index, &buffer) !=
             AUDMAIN_STATUS_SUCCESS) {
            return(AUDMAIN_STATUS_ERROR);
          } else {
            buffer_start  += index;
            index2         = 0;
          }
              index = index2;
        }
      }

      if (vrat_found == TRUE && status == QCPSUP_STATUS_SUCCESS) {
        while((status = qcpsup_find_chunk(chunk_id, buffer, &index,
                                          audqcp_buf_size,
                                          &audqcp_parse_ctl.track_length)) ==
              QCPSUP_STATUS_BUFF_ERROR) {
          if((index + buffer_start) > file_size) {
            return(AUDMAIN_STATUS_ERROR);
          }
          audmain_do_play_cb(handle, SND_CMX_AF_FREE_BUF, 0, (void *) buffer);

          if(audmain_get_data(handle, buffer_start + index, &buffer) !=
             AUDMAIN_STATUS_SUCCESS) {
            return(AUDMAIN_STATUS_ERROR);
          } else {
            buffer_start  += index;
            index          = 0;
          }
        }
      }

      if(status != QCPSUP_STATUS_SUCCESS) {
        return(AUDMAIN_STATUS_ERROR);
      }
    } else {
      audqcp_parse_ctl.format = qcpsup_validate_raw_frames(buffer,
                                                           audqcp_buf_size);

      if (audqcp_parse_ctl.format == QCPSUP_FORMAT_INVALID) {
        return(AUDMAIN_STATUS_ERROR);
      } else {
        audmain_do_play_cb(handle, SND_CMX_AF_GET_FILE_LEN, 0,
                                   (void *)&audqcp_parse_ctl.track_length);
      }
    }

    /* Initialize values */
    audqcp_parse_ctl.prompt1.prompt.pb_num_frames  = 0;
    audqcp_parse_ctl.prompt1.boarder.pb_num_frames = 0;
    audqcp_parse_ctl.prompt1.start_frame           = 0;
    audqcp_parse_ctl.prompt2.prompt.pb_num_frames  = 0;
    audqcp_parse_ctl.prompt2.boarder.pb_num_frames = 0;
    audqcp_parse_ctl.prompt2.start_frame           = 0;
    audqcp_parse_ctl.seek_ms                       = 0;
    audqcp_parse_ctl.error_status = AUDMAIN_STATUS_MAX;
    audqcp_parse_ctl.prompt1.track_buf = buffer;
    audqcp_parse_ctl.audqcp_handle     = handle;

    switch (start_position) {
      case SND_AUDFMT_START_PATH_REVERSE:
      audqcp_parse_ctl.direction    = VOC_PB_DIR_REVERSE;
        audqcp_parse_ctl.in_call      = FALSE;
        break;
      case SND_AUDFMT_START_PATH_BOTH:
      audqcp_parse_ctl.direction    = VOC_PB_DIR_BOTH;
        audqcp_parse_ctl.in_call      = FALSE;
        break;
      case SND_AUDFMT_START_PATH_BOTH_INCALL: 
        audqcp_parse_ctl.direction    = VOC_PB_DIR_REVERSE;
        audqcp_parse_ctl.in_call      = TRUE;
        break;
      case SND_AUDFMT_START_PATH_FORWARD_INCALL: 
      audqcp_parse_ctl.direction    = VOC_PB_DIR_FORWARD;
        audqcp_parse_ctl.in_call      = TRUE;
        break;
      default: 
      audqcp_parse_ctl.direction    = VOC_PB_DIR_FORWARD;
        audqcp_parse_ctl.in_call      = FALSE;
        break;
    }

    audqcp_parse_ctl.track_start                = buffer_start + index;
    audqcp_parse_ctl.prompt1.prompt.pb_data_ptr =
                                 &audqcp_parse_ctl.prompt1.track_buf[index];

    /* check integer overflow */
	if(index + audqcp_parse_ctl.track_length < audqcp_parse_ctl.track_length)
    {
      MSG_ERROR("Invalid QCP data - integer overflow",0,0,0);
      return(AUDMAIN_STATUS_ERROR);
    }

    buffer_data_size = MIN(index + audqcp_parse_ctl.track_length,
                           audqcp_buf_size);
    if(audqcp_count_frames(audqcp_parse_ctl.format, buffer,
                           &index, buffer_data_size,
                           &audqcp_parse_ctl.prompt1.prompt.pb_num_frames) ==
       AUDMAIN_STATUS_ERROR) {
      MSG_ERROR("Invalid QCP data.",0,0,0);
      return(AUDMAIN_STATUS_ERROR);
    }

#ifdef FEATURE_AUDFMT_AMR
    if(audqcp_parse_ctl.format == QCPSUP_AMR_FILE_FORMAT) {
      audqcp_parse_ctl.prompt1.prompt.pb_voc_capability = VOC_CAP_AMR;
      audqcp_parse_ctl.prompt1.prompt.pb_len_rate_var   = VOC_PB_AMR;
    }
#endif /* FEATURE_AUDFMT_AMR */

    if(audqcp_parse_ctl.format == QCPSUP_AWB_FILE_FORMAT) {
      audqcp_parse_ctl.prompt1.prompt.pb_voc_capability = VOC_CAP_AMR_WB;
      audqcp_parse_ctl.prompt1.prompt.pb_len_rate_var   = VOC_PB_AWB;
    }

#ifdef FEATURE_AUDFMT_EVB
    if(audqcp_parse_ctl.format == QCPSUP_EVB_FILE_FORMAT) {
      audqcp_parse_ctl.prompt1.prompt.pb_voc_capability = VOC_CAP_4GV_NB;
      audqcp_parse_ctl.prompt1.prompt.pb_len_rate_var   = VOC_PB_EVB;
    }
#endif /* FEATURE_AUDFMT_EVB */

#ifdef FEATURE_AUDFMT_EVW
    if(audqcp_parse_ctl.format == QCPSUP_EVW_FILE_FORMAT) {
      audqcp_parse_ctl.prompt1.prompt.pb_voc_capability = VOC_CAP_4GV_WB;
      audqcp_parse_ctl.prompt1.prompt.pb_len_rate_var   = VOC_PB_EVW;
    }
#endif /* FEATURE_AUDFMT_EVW */

    if(audqcp_parse_ctl.format == QCPSUP_FORMAT_13K) {
      audqcp_parse_ctl.prompt1.prompt.pb_voc_capability = VOC_CAP_IS733;
      audqcp_parse_ctl.prompt1.prompt.pb_len_rate_var   = VOC_PB_NATIVE_QCP;
    }
    if (audqcp_parse_ctl.format == QCPSUP_FORMAT_AMR) {
      audqcp_parse_ctl.prompt1.prompt.pb_voc_capability = VOC_CAP_AMR;
      audqcp_parse_ctl.prompt1.prompt.pb_len_rate_var   = VOC_PB_NATIVE_QCP;
    }
    if (audqcp_parse_ctl.format == QCPSUP_FORMAT_EVRC) {
      audqcp_parse_ctl.prompt1.prompt.pb_voc_capability = VOC_CAP_IS127;
      audqcp_parse_ctl.prompt1.prompt.pb_len_rate_var   = VOC_PB_NATIVE_QCP;
    }
    if (audqcp_parse_ctl.format == QCPSUP_FORMAT_4GV_NB) {
      audqcp_parse_ctl.prompt1.prompt.pb_voc_capability = VOC_CAP_4GV_NB;
      audqcp_parse_ctl.prompt1.prompt.pb_len_rate_var   = VOC_PB_NATIVE_QCP;
    }
    if (audqcp_parse_ctl.format == QCPSUP_FORMAT_EFR) {
      audqcp_parse_ctl.prompt1.prompt.pb_voc_capability = VOC_CAP_GSM_EFR;
      audqcp_parse_ctl.prompt1.prompt.pb_len_rate_var   = VOC_PB_NATIVE_QCP;
    }
    if (audqcp_parse_ctl.format == QCPSUP_FORMAT_FR) {
      audqcp_parse_ctl.prompt1.prompt.pb_voc_capability = VOC_CAP_GSM_FR;
      audqcp_parse_ctl.prompt1.prompt.pb_len_rate_var   = VOC_PB_NATIVE_QCP;
    }
    if (audqcp_parse_ctl.format == QCPSUP_FORMAT_HR) {
      audqcp_parse_ctl.prompt1.prompt.pb_voc_capability = VOC_CAP_GSM_HR;
      audqcp_parse_ctl.prompt1.prompt.pb_len_rate_var   = VOC_PB_NATIVE_QCP;
    }
    audqcp_parse_ctl.prompt1.prompt.pb_direction = audqcp_parse_ctl.direction;
    audqcp_parse_ctl.prompt1.prompt.in_call      = audqcp_parse_ctl.in_call;
    parse_ctl->current_buf.prompt1 = &(audqcp_parse_ctl.prompt1.prompt);

    audqcp_parse_ctl.file_location     = buffer_start + audqcp_buf_size;
    parse_ctl->current_buf.prompt2     = NULL;
    audqcp_parse_ctl.prompt2.track_buf = NULL;

    audqcp_parse_ctl.frame_index  = 0;
    audqcp_parse_ctl.index        = buffer_data_size - index;
    audqcp_parse_ctl.status_flag  = 0;

    if(audqcp_parse_ctl.index != 0) {
      memcpy(audqcp_parse_ctl.frame[0], 
             &audqcp_parse_ctl.prompt1.track_buf[index], 
             audqcp_parse_ctl.index);
      if(qcpsup_validate_frame(audqcp_parse_ctl.format, 
                               audqcp_parse_ctl.frame[0][0], &index)) {
        audqcp_parse_ctl.remain = index - audqcp_parse_ctl.index + 1;
      } else {
        return(AUDMAIN_STATUS_ERROR);
      }
    } else {
      audqcp_parse_ctl.remain = 0;
    }

    if((audqcp_parse_ctl.file_location - audqcp_parse_ctl.track_start) <
       (audqcp_parse_ctl.track_length)) {
      audmain_set_read_sync(FALSE);
      if(audmain_get_data(handle, audqcp_parse_ctl.file_location,
          &(audqcp_parse_ctl.prompt2.track_buf)) != AUDMAIN_STATUS_SUCCESS) {
        return(AUDMAIN_STATUS_FAILURE);
      } else {
        audqcp_buf_mgmt.first   = &audqcp_parse_ctl.prompt2;
        audqcp_buf_mgmt.cmd_buf = &parse_ctl->current_buf;
      }
    }
    audqcp_do_file_spec (handle, &audqcp_parse_ctl);

    parse_ctl->tracks            = &audqcp_parse_ctl;
    ret_val                      = AUDMAIN_STATUS_SUCCESS;
    audqcp_parse_ctl.status      = AUDQCP_STATUS_PARSE_HEAD;
    audqcp_parse_ctl.curr_prompt = &audqcp_parse_ctl.prompt1;
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audqcp_age

DESCRIPTION
  This function ages the file by the indicated number of milliseconds.
  No aging is necessary for QCP files because the timing is driven by the
  vocoder.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audqcp_age (
  audmain_parse_ctl_type  *parse_ctl,
  uint32                  *elapsed_time,
  uint32                  *adjusted_time
    /*lint -esym(715,elapsed_time) Tell Lint we know elapsed_time 
      parameter is not used */
    /*lint -esym(715,adjusted_time) Tell Lint we know adjusted_time 
      parameter is not used */
) {

}

/* <EJECT> */
/*===========================================================================

FUNCTION audqcp_parse

DESCRIPTION
  This function parses one event of the QCP file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audqcp_parse (
  audmain_parse_ctl_type  *parse_ctl,
  audmain_cmd_buf_type    *cmd_buf,
  uint32                  *time_remain
    /*lint -esym(715,time_remain) Tell Lint we know time_remain 
      parameter is not used */
) {
  audmain_status_type   ret_val = AUDMAIN_STATUS_FAILURE;
  audqcp_parse_ctl_type *qcp_ptr;
  audqcp_prompt_type    *prompt = NULL;

  if(parse_ctl != NULL) {
    qcp_ptr                 = (audqcp_parse_ctl_type *) parse_ctl->tracks;

    if (!(qcp_ptr->status_flag & AUDQCP_MASK_FAILURE)) {
      if(cmd_buf->prompt1 == &qcp_ptr->prompt1.prompt) {
        prompt = &qcp_ptr->prompt1;
      } else if(cmd_buf->prompt1 == &qcp_ptr->prompt2.prompt) {
        prompt = &qcp_ptr->prompt2;
      }
      cmd_buf->prompt1 = NULL;
      audqcp_parse_ctl.status = AUDQCP_STATUS_PLAY;

      ret_val = audqcp_get_data(qcp_ptr, cmd_buf, prompt);
    } else {
      ret_val = qcp_ptr->error_status;
    }
  } else {
    MSG_ERROR("Null buffer pointer", 0,0,0);
  }
  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audqcp_seek

DESCRIPTION
  This function moves the playback to the proper position.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_SUCCESS - if no errors in the prompt were found
  AUDMAIN_STATUS_ERROR   - if there is an invalid frame in the prompt

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audqcp_seek ( 
  qcpsup_format_enum_type format,
  voc_pb_header_type      *vp,
  uint32                  *num_frames
) {
  audmain_status_type ret_val = AUDMAIN_STATUS_ERROR;
  uint32              frame_size, frame_count;
  uint8               *buffer;

  frame_count = *num_frames;

  if(frame_count < vp->pb_num_frames) {
    vp->pb_num_frames -= frame_count;
    buffer             = vp->pb_data_ptr;

    while((frame_count > 0) &&
                        qcpsup_validate_frame(format, *buffer, &frame_size)) {
      buffer += frame_size + 1;
      frame_count--;
    }

    if(frame_count == 0) {
      vp->pb_data_ptr = buffer;
      ret_val         = AUDMAIN_STATUS_SUCCESS;
    }
  } else {
    frame_count       -= vp->pb_num_frames;
    vp->pb_num_frames  = 0;
    ret_val            = AUDMAIN_STATUS_SUCCESS;
  }

  *num_frames = frame_count;
  return (ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audqcp_locate_ms

DESCRIPTION
  This function determines if the elapsed frames is found within the
  indicated prompt

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_SUCCESS - if elapsed frames is within the prompt
  AUDMAIN_STATUS_DONE    - if elapsed frames is beyond the prompt

SIDE EFFECTS
  May request next data buffer.

===========================================================================*/
audmain_status_type audqcp_locate_ms (
  audqcp_parse_ctl_type  *qcp_ctl,
  audmain_cmd_buf_type   *cmd_buf,
  audqcp_prompt_type     *prompt,
  uint32                 elapsed_frames,
  uint32                 *elapsed_ms
) {
  audmain_status_type ret_val;
  uint32              end_frames, new_elapsed_ms, diff_frames;
  uint32              diff_ms;

  end_frames = prompt->start_frame + prompt->prompt.pb_num_frames +
               prompt->boarder.pb_num_frames;

  if(elapsed_frames < end_frames) {
    if(prompt->track_buf != NULL) {
      diff_frames = elapsed_frames - prompt->start_frame;
      if (diff_frames) {
        prompt->start_frame           += diff_frames;
        diff_frames                   -= prompt->boarder.pb_num_frames;
        prompt->boarder.pb_num_frames  = 0;
        ret_val = audqcp_seek(qcp_ctl->format, &prompt->prompt, &diff_frames);
      } else {
        ret_val = AUDMAIN_STATUS_SUCCESS;
      }
    } else {
      MSG_HIGH("ET << actual time",0,0,0);
      new_elapsed_ms = end_frames * AUDQCP_FRAME_TIME;
      diff_ms        = new_elapsed_ms - (*elapsed_ms);
      if (diff_ms < qcp_ctl->seek_ms) {
        qcp_ctl->seek_ms -= diff_ms;
      } else {
        qcp_ctl->seek_ms  = 0;
      }
      (*elapsed_ms)     = new_elapsed_ms;
      ret_val = AUDMAIN_STATUS_SUCCESS;
    }
  } else {
    (void) audqcp_get_data(qcp_ctl, cmd_buf, prompt);
    ret_val = AUDMAIN_STATUS_DONE;
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audqcp_seek_frames

DESCRIPTION
  This function seeks the number of frames within the prompt.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_PENDING - if prompt is less than total #frames
  AUDMAIN_STATUS_SUCCESS - if prompt has more than total #frames
  AUDMAIN_STATUS_DONE    - if we are at the end of the file

SIDE EFFECTS
  May request next buffer in file

===========================================================================*/
audmain_status_type audqcp_seek_frames (
  audqcp_prompt_type     *prompt,
  audqcp_parse_ctl_type  *qcp_ctl,
  audmain_cmd_buf_type   *cmd_buf,
  uint32                 *frames
) {
  audmain_status_type ret_val = AUDMAIN_STATUS_PENDING;
  uint32              prompt_frames;

  if(prompt->track_buf != NULL) {
    prompt_frames = prompt->prompt.pb_num_frames + 
                                          prompt->boarder.pb_num_frames;
    if(*frames < prompt_frames) {
      prompt->start_frame           += *frames;
      *frames                       -= prompt->boarder.pb_num_frames;
      prompt->boarder.pb_num_frames  = 0;
      (void) audqcp_seek(qcp_ctl->format, &prompt->prompt, frames);
      ret_val = AUDMAIN_STATUS_SUCCESS;
    } else {
      *frames -= prompt_frames;
      ret_val  = audqcp_get_data(qcp_ctl, cmd_buf, prompt);
      if(ret_val != AUDMAIN_STATUS_DONE) {
        ret_val = AUDMAIN_STATUS_PENDING;
      }
    }
  } else if((qcp_ctl->file_location - qcp_ctl->track_start) >=
             qcp_ctl->track_length) {
    ret_val = AUDMAIN_STATUS_DONE;
  }
  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audqcp_seek_current_time

DESCRIPTION
  This function moves the playback to the current elapsed time.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audqcp_seek_current_time (
  audqcp_parse_ctl_type *qcp_ctl,
  audmain_cmd_buf_type  *cmd_buf,
  uint32                *elapsed_ms
) {
  audqcp_prompt_type *prompt1, *prompt2;
  uint32             elapsed_frames;
  uint32             end_frames;
  uint32             new_elapsed_ms;
  uint32             diff_ms;

  if((qcp_ctl != NULL) && (elapsed_ms != NULL)) {
    if(qcp_ctl->prompt1.start_frame < qcp_ctl->prompt2.start_frame) {
      prompt1 = &qcp_ctl->prompt1;
      prompt2 = &qcp_ctl->prompt2;
    } else {
      prompt1 = &qcp_ctl->prompt2;
      prompt2 = &qcp_ctl->prompt1;
    }

    elapsed_frames  = (*elapsed_ms)/AUDQCP_FRAME_TIME;

    if (elapsed_frames < prompt1->start_frame) {
      new_elapsed_ms = prompt1->start_frame * AUDQCP_FRAME_TIME;
      diff_ms        = new_elapsed_ms - (*elapsed_ms);
      if (diff_ms < qcp_ctl->seek_ms) {
        qcp_ctl->seek_ms -= diff_ms;
      } else {
        qcp_ctl->seek_ms  = 0;
      }
      (*elapsed_ms)     = new_elapsed_ms;
      if((prompt1->track_buf == NULL) || (prompt2->track_buf == NULL)) {
        MSG_HIGH("ET <<< actual time", 0,0,0);
      }
    } else if((audqcp_locate_ms(qcp_ctl, cmd_buf, prompt1, elapsed_frames,
                                elapsed_ms) != AUDMAIN_STATUS_SUCCESS) &&
              (audqcp_locate_ms(qcp_ctl, cmd_buf, prompt2, elapsed_frames, 
                                elapsed_ms) != AUDMAIN_STATUS_SUCCESS)) {
      end_frames = prompt2->start_frame + prompt2->prompt.pb_num_frames +
                   prompt2->boarder.pb_num_frames;

      new_elapsed_ms    = end_frames * AUDQCP_FRAME_TIME;
      qcp_ctl->seek_ms += (*elapsed_ms) - new_elapsed_ms;
      (*elapsed_ms)     = new_elapsed_ms;
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION audqcp_fforward

DESCRIPTION
  This function moves the playback forward ms milliseconds.
  Each data frame is 20 ms.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audqcp_fforward (
  audmain_parse_ctl_type *parse_ctl,
  uint32                 ms_forward,
  uint32                 *elapsed_ms,
  uint32                 *adjusted_ms
) {
  audmain_status_type    ret_val = AUDMAIN_STATUS_ERROR;
  audqcp_parse_ctl_type  *qcp_ctl;

  if(parse_ctl != NULL) {
    qcp_ctl           = (audqcp_parse_ctl_type *) parse_ctl->tracks;
    qcp_ctl->seek_ms += ms_forward;
    ret_val           = AUDMAIN_STATUS_SUCCESS;

    if(!(parse_ctl->status_flag & AUDMAIN_MASK_FF_PAUSE)) {
      audqcp_seek_current_time(qcp_ctl, &parse_ctl->current_buf, elapsed_ms);

      (*adjusted_ms) = (*elapsed_ms);
      parse_ctl->status_flag |= AUDMAIN_MASK_FF_PAUSE;
    }
  }
  return(ret_val);
} /* end audqcp_fforward */

/* <EJECT> */
/*===========================================================================

FUNCTION audqcp_restart

DESCRIPTION
  This function restart the QCP file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audqcp_restart (
  audmain_parse_ctl_type *parse_ctl
) {
  audmain_status_type   af_status = AUDMAIN_STATUS_ERROR;
  audqcp_parse_ctl_type *qcp_ctl  = NULL;
  audmain_cmd_buf_type  *cmd_buf;
  audqcp_prompt_type    *prompt1, *prompt2;

  if (parse_ctl != NULL) {
    qcp_ctl = (audqcp_parse_ctl_type *) parse_ctl->tracks;

    cmd_buf          = &parse_ctl->current_buf;
    cmd_buf->prompt1 = NULL;
    cmd_buf->prompt2 = NULL;
    cmd_buf->prompt3 = NULL;
    cmd_buf->prompt4 = NULL;

    if(audqcp_buf_mgmt.first != NULL) {
      audqcp_buf_mgmt.skip = audqcp_buf_mgmt.first;
    }

    qcp_ctl->file_location = qcp_ctl->track_start;
    qcp_ctl->status_flag  |= AUDQCP_MASK_RESTART;
    qcp_ctl->index         = 0;
    qcp_ctl->remain        = 0;

    prompt1 = &qcp_ctl->prompt1;
    prompt2 = &qcp_ctl->prompt2;

    prompt1->prompt.pb_num_frames  = 0;
    prompt1->boarder.pb_num_frames = 0;
    prompt1->start_frame           = 0;
    prompt2->prompt.pb_num_frames  = 0;
    prompt2->boarder.pb_num_frames = 0;
    prompt2->start_frame           = 0;

    if(prompt1->track_buf != NULL) {
      audmain_do_play_cb(qcp_ctl->audqcp_handle, SND_CMX_AF_FREE_BUF, 0,
                         (void *) prompt1->track_buf);
      prompt1->track_buf = NULL;
    }

    if(prompt2->track_buf != NULL) {
      audmain_do_play_cb(qcp_ctl->audqcp_handle, SND_CMX_AF_FREE_BUF, 0,
                         (void *) prompt2->track_buf);
      prompt2->track_buf = NULL;
    }

    audqcp_buf_mgmt.first   = prompt1;
    audqcp_buf_mgmt.second  = prompt2;
    audqcp_buf_mgmt.cmd_buf = cmd_buf;

    if (audqcp_buf_mgmt.skip == NULL) {
      /* Only request for new buffer if there is not pending buffer 
      */
      (void) audmain_get_data(qcp_ctl->audqcp_handle, qcp_ctl->track_start, 
                              &qcp_ctl->prompt1.track_buf);
    }

    af_status = AUDMAIN_STATUS_SUCCESS;
    
  }
  return (af_status);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audqcp_rewind

DESCRIPTION
  This function sets up the QCP file to rewind the number of ms indicated.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audqcp_rewind (
  audmain_parse_ctl_type *parse_ctl,
  uint32                 ms_rewind,
  uint32                 *elapsed_ms,
  uint32                 *adjusted_ms
) {
  audmain_status_type        ret_val = AUDMAIN_STATUS_ERROR;
  audqcp_parse_ctl_type      *qcp_ctl;
  boolean                    paused;

  if(parse_ctl != NULL) {
    qcp_ctl = (audqcp_parse_ctl_type *) parse_ctl->tracks;

    /* If we rewinding when it is doing forwarding
    ** just subtract the differents.
    */
    if (ms_rewind < qcp_ctl->seek_ms) {
      qcp_ctl->seek_ms -= ms_rewind;
      ret_val           = AUDMAIN_STATUS_SUCCESS;
    } else if (ms_rewind == qcp_ctl->seek_ms) {
      parse_ctl->status_flag |= AUDMAIN_MASK_FF_PAUSE;
      qcp_ctl->seek_ms        = 0;

      audqcp_seek_current_time(qcp_ctl, &parse_ctl->current_buf, elapsed_ms);
      *adjusted_ms = *elapsed_ms;
      ret_val      = AUDMAIN_STATUS_SUCCESS;
    } else {
      /* Subtract fast-forward time from rewind time
      */
      ms_rewind -= qcp_ctl->seek_ms;

      paused = parse_ctl->status_flag & AUDMAIN_MASK_PAUSE;

      if((qcp_ctl->status_flag & AUDQCP_MASK_RESTART) ||
         (ret_val = audqcp_restart(parse_ctl)) == AUDMAIN_STATUS_SUCCESS) {
        parse_ctl->status_flag |= AUDMAIN_MASK_FF_PAUSE;

        if(paused) {
          parse_ctl->status_flag |= AUDMAIN_MASK_PAUSE;
        }

        if(ms_rewind >= (*elapsed_ms)) {
          qcp_ctl->seek_ms = 0;
        } else {
          qcp_ctl->seek_ms = (*elapsed_ms) - ms_rewind;
        }
        *elapsed_ms  = 0;
        *adjusted_ms = 0;
      }
    }
  }
  return(ret_val);
} /* end audqcp_rewind */

/* <EJECT> */
/*===========================================================================

FUNCTION audqcp_do_seek

DESCRIPTION
  This function  moves the playback to the proper position.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_PENDING - if still seeking through file
  AUDMAIN_STATUS_SUCCESS - if position in file has been found
  AUDMAIN_STATUS_DONE    - if end of file has been reached

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audqcp_do_seek (
  audmain_parse_ctl_type *parse_ctl,
  audmain_cmd_buf_type   *cmd_buf,
  uint32                 *elapsed_ms,
  uint32                 *adjusted_ms
) {
  audmain_status_type   af_status = AUDMAIN_STATUS_PENDING;
  audqcp_parse_ctl_type *qcp_ctl  = NULL;
  audqcp_prompt_type    *prompt1, *prompt2;
  uint32                seek_frames = 0;
  uint32                remain_ms;

  if (parse_ctl != NULL) {
    qcp_ctl = (audqcp_parse_ctl_type *) parse_ctl->tracks;

    if(qcp_ctl->prompt1.start_frame < qcp_ctl->prompt2.start_frame) {
      prompt1 = &qcp_ctl->prompt1;
      prompt2 = &qcp_ctl->prompt2;
    } else {
      prompt1 = &qcp_ctl->prompt2;
      prompt2 = &qcp_ctl->prompt1;
    }

    if (qcp_ctl->status_flag & AUDQCP_MASK_FAILURE) {
      af_status = AUDMAIN_STATUS_FAILURE;
    } else {
      seek_frames = qcp_ctl->seek_ms / AUDQCP_FRAME_TIME;

      if(seek_frames > 0) {
        af_status = audqcp_seek_frames(prompt1, qcp_ctl, cmd_buf, &seek_frames);
        if(af_status == AUDMAIN_STATUS_DONE) {
          if(seek_frames == 0) {
            af_status = AUDMAIN_STATUS_SUCCESS;
          } else {
            af_status = AUDMAIN_STATUS_PENDING;
          }
        }
        if((seek_frames > 0) && (af_status == AUDMAIN_STATUS_PENDING)) {
          af_status = audqcp_seek_frames(prompt2, qcp_ctl, cmd_buf, 
                                         &seek_frames);
        }
      } else {
        af_status = AUDMAIN_STATUS_SUCCESS;
      }
    }

    cmd_buf->prompt1 = NULL;
    cmd_buf->prompt2 = NULL;
    cmd_buf->prompt3 = NULL;
    cmd_buf->prompt4 = NULL;

    if((seek_frames == 0) || (af_status != AUDMAIN_STATUS_PENDING)) {
      if (af_status == AUDMAIN_STATUS_DONE) {
        *elapsed_ms += qcp_ctl->seek_ms - (seek_frames * AUDQCP_FRAME_TIME);
      } else {
        *elapsed_ms += qcp_ctl->seek_ms;
      }
      qcp_ctl->seek_ms        = 0;
      parse_ctl->status_flag &= ~AUDMAIN_MASK_FF_PAUSE;

      if (af_status == AUDMAIN_STATUS_SUCCESS) {
        if(prompt1->track_buf != NULL) {
          if(prompt1->boarder.pb_num_frames != 0) {
            cmd_buf->prompt1   = &prompt1->boarder;
          }
          if(prompt1->prompt.pb_num_frames != 0) {
            cmd_buf->prompt2   = &prompt1->prompt;
          }
          if(prompt2->track_buf != NULL) {
            if(prompt2->boarder.pb_num_frames != 0) {
              cmd_buf->prompt3 = &prompt2->boarder;
            }
            if(prompt2->prompt.pb_num_frames != 0) {
              cmd_buf->prompt4 = &prompt2->prompt;
            }
          }
        } else if(prompt2->track_buf != NULL) {
          if(prompt2->boarder.pb_num_frames != 0) {
            cmd_buf->prompt1   = &prompt2->boarder;
          }
          if(prompt2->prompt.pb_num_frames != 0) {
            cmd_buf->prompt2   = &prompt2->prompt;
          }
        }
      } else {
        /* Generate a Zero frame prompt to done the vocoder
        */
        prompt1->boarder.pb_num_frames = 0;
        cmd_buf->prompt1               = &prompt1->boarder;
      }
    } else {
      remain_ms         = seek_frames * AUDQCP_FRAME_TIME;
      *elapsed_ms      += qcp_ctl->seek_ms - remain_ms;
      qcp_ctl->seek_ms  = remain_ms;
    }
    *adjusted_ms      = *elapsed_ms;
  }
  return (af_status);
} /* end audqcp_do_seek */

/* <EJECT> */
/*===========================================================================

FUNCTION audqcp_flush

DESCRIPTION
  This function flushes the QCP buffers and requests more data from client.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audqcp_flush (
  audmain_parse_ctl_type *parse_ctl
) {
  return(audqcp_restart(parse_ctl));
}

/* <EJECT> */
/*===========================================================================

FUNCTION audqcp_func_req

DESCRIPTION
  This function does the get vrat or data processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audqcp_func_req (
  snd_af_access_type    *handle,
  uint8                 *buffer,
  uint32                position,
  uint32                buf_len
) {
  qcpsup_status_type status = QCPSUP_STATUS_SUCCESS;
  uint32             index = 0, size = 0;
  uint8              riff_id[]  = {'Q', 'L', 'C', 'M'};
  uint8              chunk_id[] = {'d', 'a', 't', 'a'};

  if(audqcp_func_file_size == 0) {
    if(qcpsup_do_riff(riff_id, buffer, &index, &audqcp_func_file_size) != 
       QCPSUP_STATUS_SUCCESS) {
      status = QCPSUP_STATUS_ERROR;
    }
  } 

  if(status != QCPSUP_STATUS_ERROR) {
    if(audqcp_func_status == AUDQCP_FUNC_GET_DATA) {
      status = qcpsup_find_chunk(chunk_id, buffer, &index, buf_len, &size);
    } else if(audqcp_func_status == AUDQCP_FUNC_CNFG) {
      status = qcpsup_get_cnfg(buffer, &index, buf_len, &size);
    } else {
      status = qcpsup_get_vrat(buffer, &index, buf_len, &size);
    }
  }

  if(status == QCPSUP_STATUS_BUFF_ERROR) {
    if((index + position) < audqcp_func_file_size) {
      if((handle != NULL) && (handle->data_func != NULL)) {
        handle->data_func(SND_CMX_AF_FREE_BUF, handle->audfmt_ptr, 0,
                          (void *) buffer);
      }
      audqcp_func_handle = handle;
      if(audmain_get_data(handle, position + index, &audqcp_func_buffer) !=
         AUDMAIN_STATUS_SUCCESS) {
        audqcp_func_handle = NULL;
        status             = QCPSUP_STATUS_ERROR;
      }
    } else {
      status = QCPSUP_STATUS_ERROR;
    }
  }

  if(status == QCPSUP_STATUS_SUCCESS) {
    if((handle != NULL) && (handle->data_func != NULL)) {
      index += position;
      handle->data_func(SND_CMX_QCP_SUCCESS, handle->audfmt_ptr, size, &index);
    }
    audmain_reset_qcp_func();
  }

  if(status == QCPSUP_STATUS_ERROR) {
    if((handle != NULL) && (handle->data_func != NULL)) {
      handle->data_func(SND_CMX_QCP_FAILURE, handle->audfmt_ptr, 0, NULL);
    }
    audmain_reset_qcp_func();
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION audqcp_get_func

DESCRIPTION
  This function finds the vrat or data chunk in a QCP file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audqcp_get_func (
  audqcp_func_type       func,
  snd_af_access_type     *handle,
  audmain_parse_ctl_type *parse_ctl
) {
  if(!parse_ctl->qcp_func_enabled) {
    if((func == AUDQCP_FUNC_GET_DATA) || (func == AUDQCP_FUNC_GET_VRAT) ||
       (func == AUDQCP_FUNC_CNFG)) {
      if(audmain_get_data(handle, 0, &audqcp_func_buffer) !=
         AUDMAIN_STATUS_SUCCESS) {
        parse_ctl->type = AUDMAIN_INVALID_TYPE;
      } else {
        audqcp_func_file_size       = 0;
        audqcp_func_handle          = handle;
        audqcp_func_status          = func;
        parse_ctl->qcp_func_enabled = TRUE;
        return;
      }
    }
  }

  if((handle != NULL) && (handle->data_func != NULL)) {
    handle->data_func(SND_CMX_QCP_FAILURE, handle->audfmt_ptr, 0, NULL);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION audqcp_chunk_req

DESCRIPTION
  This function does the get/set label/text and set cnfg processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audqcp_chunk_req (
  snd_af_access_type *handle,
  uint8              *buffer,
  uint32             position,
  uint32             buf_len
) {
  snd_qcp_rec_data_struct_type qcp_data;
  qcpsup_status_type           status = QCPSUP_STATUS_SUCCESS;
  uint32                       index = 0, length = 0;
  uint8                        riff_id[] = {'Q', 'L', 'C', 'M'};
  uint8                        labl_id[] = {'l', 'a', 'b', 'l'};
  uint8                        text_id[] = {'t', 'e', 'x', 't'};
  uint8                        cnfg_id[] = {'c', 'n', 'f', 'g'};

  static uint32 file_size, chunk_bytes;

  if(audqcp_func_file_size == 0) {
    if(qcpsup_do_riff(riff_id, buffer, &index, &audqcp_func_file_size) != 
       QCPSUP_STATUS_SUCCESS) {
      status = QCPSUP_STATUS_ERROR;
    }
  } 

  if(status != QCPSUP_STATUS_ERROR) {
    if(audqcp_func_status == AUDQCP_FUNC_CNFG) {
      status = qcpsup_find_chunk(cnfg_id, buffer, &index, buf_len, &length);
    } else if(audqcp_func_status == AUDQCP_FUNC_LABL) {
      status = qcpsup_find_chunk(labl_id, buffer, &index, buf_len, &length);
    } else {
      status = qcpsup_find_chunk(text_id, buffer, &index, buf_len, &length);
    }
  }

  if(status == QCPSUP_STATUS_BUFF_ERROR) {
    if((index + position) < audqcp_func_file_size) {
      if((handle != NULL) && (handle->data_func != NULL)) {
        handle->data_func(SND_CMX_AF_FREE_BUF, handle->audfmt_ptr, 0,
                          (void *) buffer);
      }
      audqcp_get_chunk_handle = handle;
      if(audmain_get_data(handle, position + index, &audqcp_func_buffer) !=
         AUDMAIN_STATUS_SUCCESS) {
        audqcp_get_chunk_handle = NULL;
        status                  = QCPSUP_STATUS_ERROR;
      }
    } else if(audqcp_set_chunk_cb_func != NULL) {
      file_size = audqcp_func_file_size + audqcp_set_chunk_num_bytes;
      if(audqcp_set_chunk_num_bytes & 0x1) {
        file_size++;
      }
      qcp_data.buffer    = (uint8 *) &file_size;
      qcp_data.position  = 4;
      qcp_data.num_bytes = 4;
      qcp_data.content   = SND_QCP_REC_DATA_BUFFER_UNCATAGORIZED;
      audqcp_set_chunk_cb_func(SND_CMX_QCP_REC_DATA, &qcp_data,
                                          audqcp_set_chunk_client_data);

      if(audqcp_func_status == AUDQCP_FUNC_CNFG) {
        qcp_data.buffer  = (uint8 *) audqcp_cnfg_chunk;
      } else if(audqcp_func_status == AUDQCP_FUNC_LABL) {
        qcp_data.buffer  = (uint8 *) audqcp_labl_chunk;
      } else {
        qcp_data.buffer  = (uint8 *) audqcp_text_chunk;
      }
      qcp_data.position  = audqcp_func_file_size;
      qcp_data.num_bytes = 4;
      qcp_data.content   = SND_QCP_REC_DATA_BUFFER_UNCATAGORIZED;
      audqcp_set_chunk_cb_func(SND_CMX_QCP_REC_DATA, &qcp_data,
                                          audqcp_set_chunk_client_data);

      chunk_bytes        = audqcp_set_chunk_num_bytes;
      qcp_data.buffer    = (uint8 *) &chunk_bytes;
      qcp_data.position  = audqcp_func_file_size + 4;
      qcp_data.num_bytes = 4;
      qcp_data.content   = SND_QCP_REC_DATA_BUFFER_UNCATAGORIZED;
      audqcp_set_chunk_cb_func(SND_CMX_QCP_REC_DATA, &qcp_data, 
                                          audqcp_set_chunk_client_data);

      qcp_data.buffer    = audqcp_set_chunk_buffer_ptr;
      qcp_data.position  = audqcp_func_file_size + 4 + 4;
      qcp_data.num_bytes = audqcp_set_chunk_num_bytes;
      qcp_data.content   = SND_QCP_REC_DATA_BUFFER_UNCATAGORIZED;
      audqcp_set_chunk_cb_func(SND_CMX_QCP_REC_DATA, &qcp_data,
                                        audqcp_set_chunk_client_data);

      if(audqcp_set_chunk_num_bytes & 1) {
        qcp_data.buffer    = (uint8 *) &audqcp_pad_byte;
        qcp_data.position  = audqcp_func_file_size + 4 + 4 +
                                       audqcp_set_chunk_num_bytes;
        qcp_data.num_bytes = 1;
        qcp_data.content   = SND_QCP_REC_DATA_BUFFER_UNCATAGORIZED;
        audqcp_set_chunk_cb_func(SND_CMX_QCP_REC_DATA, &qcp_data,
                                        audqcp_set_chunk_client_data);
      }

      audqcp_set_chunk_cb_func(SND_CMX_QCP_SUCCESS, &qcp_data,
                                        audqcp_set_chunk_client_data);

      audqcp_set_chunk_cb_func = NULL;
      audmain_reset_qcp_func();
    } else {
      status = QCPSUP_STATUS_ERROR;
    }
  }

  if(status == QCPSUP_STATUS_SUCCESS) {
    if(audqcp_set_chunk_cb_func != NULL) {
      qcp_data.buffer    = audqcp_set_chunk_buffer_ptr;
      qcp_data.position  = position + index;
      qcp_data.num_bytes = MIN(audqcp_set_chunk_num_bytes, length);
      qcp_data.content   = SND_QCP_REC_DATA_BUFFER_UNCATAGORIZED;

      audqcp_set_chunk_cb_func(SND_CMX_QCP_REC_DATA, &qcp_data, 
                                        audqcp_set_chunk_client_data);

      audqcp_set_chunk_cb_func(SND_CMX_QCP_SUCCESS, &qcp_data, 
                                        audqcp_set_chunk_client_data);

      audqcp_set_chunk_cb_func = NULL;
      audmain_reset_qcp_func();
    } else if((index + length) > buf_len) {
      if((handle != NULL) && (handle->data_func != NULL)) {
        handle->data_func(SND_CMX_AF_FREE_BUF, handle->audfmt_ptr, 0,
                          (void *) buffer);
      }
      audqcp_get_chunk_handle = handle;
      audqcp_get_chunk_length = length;
      if(audmain_get_data(handle, position + index, &audqcp_func_buffer) !=
         AUDMAIN_STATUS_SUCCESS) {
        audqcp_get_chunk_handle = NULL;
        status                  = QCPSUP_STATUS_ERROR;
      }
    } else {
      if((handle != NULL) && (handle->data_func != NULL)) {
        handle->data_func(SND_CMX_QCP_SUCCESS, handle->audfmt_ptr,
                                                      length, &buffer[index]);
      }
      audmain_reset_qcp_func();
    }
  }

  if(status == QCPSUP_STATUS_ERROR) {
    if(audqcp_set_chunk_cb_func != NULL) {
      audqcp_set_chunk_cb_func(SND_CMX_QCP_FAILURE, NULL,
                                     audqcp_set_chunk_client_data);
      audqcp_set_chunk_cb_func = NULL;
    } else {
      if((handle != NULL) && (handle->data_func != NULL)) {
        handle->data_func(SND_CMX_QCP_FAILURE, handle->audfmt_ptr, 0, NULL);
      }
      audmain_reset_qcp_func();
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION audqcp_get_chunk_data

DESCRIPTION
  This function finds the chunk in a QCP file and returns the data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audqcp_get_chunk_data (
  audqcp_func_type       func,
  snd_af_access_type     *handle,
  audmain_parse_ctl_type *parse_ctl
) {
  if(!parse_ctl->qcp_func_enabled) {
    if((func == AUDQCP_FUNC_CNFG) || (func == AUDQCP_FUNC_LABL) ||
       (func == AUDQCP_FUNC_TEXT)) {
      if(audmain_get_data(handle, 0, &audqcp_func_buffer) !=
         AUDMAIN_STATUS_SUCCESS) {
        parse_ctl->type = AUDMAIN_INVALID_TYPE;
      } else {
        audqcp_get_chunk_length     = 0;
        audqcp_func_file_size       = 0;
        audqcp_func_status          = func;
        audqcp_get_chunk_handle     = handle;
        parse_ctl->qcp_func_enabled = TRUE;
        return;
      }
    }
  }

  if(audqcp_set_chunk_cb_func != NULL) {
    audqcp_set_chunk_cb_func(SND_CMX_QCP_FAILURE, NULL,
                                   audqcp_set_chunk_client_data);
    audqcp_set_chunk_cb_func = NULL;
  } else if((handle != NULL) && (handle->data_func != NULL)) {
    handle->data_func(SND_CMX_QCP_FAILURE, handle->audfmt_ptr, 0, NULL);
  }
}

/* <EJECT> */
/*===========================================================================
 
FUNCTION audqcp_set_chunk_data

DESCRIPTION
  This function sets the chunk in a QCP file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audqcp_set_chunk_data (
  audqcp_func_type               func,
  audmain_parse_ctl_type         *parse_ctl,
  uint8                          *buffer,
  uint32                         num_bytes,
  snd_af_access_type             *handle,
  snd_qcp_rec_cb_func_ptr_type   cb_func,
  const void                     *client_data
) {
  if(!parse_ctl->qcp_func_enabled) {
    if((func == AUDQCP_FUNC_CNFG) || (func == AUDQCP_FUNC_LABL) ||
       (func == AUDQCP_FUNC_TEXT)) {
      if(handle != NULL) {
        if(func == AUDQCP_FUNC_LABL) {
          memset(audqcp_set_labl_buffer, 0, AUDQCP_LABL_SIZE);
          memcpy(audqcp_set_labl_buffer, buffer, 
                                    MIN(num_bytes, AUDQCP_LABL_SIZE));
          audqcp_set_chunk_buffer_ptr = audqcp_set_labl_buffer;
          audqcp_set_chunk_num_bytes  = AUDQCP_LABL_SIZE;
        } else {
          audqcp_set_chunk_buffer_ptr = buffer;
          audqcp_set_chunk_num_bytes  = num_bytes;
        }
        audqcp_set_chunk_cb_func     = cb_func;
        audqcp_set_chunk_client_data = client_data;

        audqcp_get_chunk_data (func, handle, parse_ctl);
        return;
      }
    }
  }

  if(cb_func != NULL) {
    cb_func(SND_CMX_QCP_FAILURE, NULL, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION audqcp_record

DESCRIPTION
  This function starts a QCP recording session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audqcp_record (
  snd_qcp_format_type          format,
  snd_qcp_dir_type             link,
  snd_qcp_rec_cb_func_ptr_type cb_func,
  const void                   *client_data
) {
  audmain_status_type ret_val = AUDMAIN_STATUS_FAILURE;
  snd_qcp_rec_data_struct_type qcp_data;

  audqcp_rec_format = QCPSUP_FORMAT_INVALID;

  if(format < SND_QCP_FORMAT_INVALID) {
    if((format == SND_QCP_FORMAT_VAR_FULL_EVRC) ||
       (format == SND_QCP_FORMAT_FIXED_FULL_EVRC)) {
      audqcp_rec_format  = QCPSUP_FORMAT_EVRC;
    }
    if((format == SND_QCP_FORMAT_VAR_FULL_4GV_NB) || 
       (format == SND_QCP_FORMAT_FIXED_FULL_4GV_NB)) {
      audqcp_rec_format  = QCPSUP_FORMAT_4GV_NB;
    }
    if (((format >= SND_QCP_FORMAT_FIXED_4_75_AMR) &&
         (format <= SND_QCP_FORMAT_FIXED_12_2_AMR)) ||
        ((format >= SND_QCP_FORMAT_FIXED_4_75_AMR_DTX) &&
         (format <= SND_QCP_FORMAT_FIXED_12_2_AMR_DTX))
        ) {
      audqcp_rec_format  = QCPSUP_FORMAT_AMR;
    }
    if ((format == SND_QCP_FORMAT_FIXED_FULL_13K) ||
        (format == SND_QCP_FORMAT_VAR_FULL_13K) ||
        (format == SND_QCP_FORMAT_FIXED_HALF_13K) ||
        (format == SND_QCP_FORMAT_VAR_HALF_13K)) {
      audqcp_rec_format  = QCPSUP_FORMAT_13K;
    }
#ifdef FEATURE_AUDFMT_AMR
    if (((format >= SND_AMR_FORMAT_FIXED_4_75_AMR) &&
         (format <= SND_AMR_FORMAT_FIXED_12_2_AMR)) ||
        ((format >= SND_AMR_FORMAT_FIXED_4_75_AMR_DTX) &&
         (format <= SND_AMR_FORMAT_FIXED_12_2_AMR_DTX))
        ) {
      audqcp_rec_format  = QCPSUP_AMR_FILE_FORMAT;
    }
#endif /* FEATURE_AUDFMT_AMR */
#ifdef FEATURE_AUDFMT_EVB
    if (format == SND_EVB_FORMAT_VAR_FULL_4GV_NB) {
      audqcp_rec_format  = QCPSUP_EVB_FILE_FORMAT;
    }
#endif /* FEATURE_AUDFMT_EVB */

#ifdef FEATURE_AUDFMT_EVW
    if (format ==   SND_EVW_FORMAT_VAR_FULL_4GV_WB) {
      audqcp_rec_format  = QCPSUP_EVW_FILE_FORMAT;
    }
#endif /* FEATURE_AUDFMT_EVW */

    if (format == SND_QCP_FORMAT_FIXED_FULL_EFR) {
      audqcp_rec_format  = QCPSUP_FORMAT_EFR;
    }
    if (format == SND_QCP_FORMAT_FIXED_FULL_FR) {
      audqcp_rec_format  = QCPSUP_FORMAT_FR;
    }
    if (format == SND_QCP_FORMAT_FIXED_FULL_HR) {
      audqcp_rec_format  = QCPSUP_FORMAT_HR;
    }

    audqcp_rec_link      = link;
    audqcp_rec_rx_bytes  = 0;
    audqcp_rec_rx_frames = 0;
    audqcp_rec_tx_bytes  = 0;
    audqcp_rec_tx_frames = 0;
    audqcp_rec_cb        = cb_func;
    audqcp_client_data   = client_data;

    if(cb_func != NULL) {
      if(audqcp_rec_format == QCPSUP_FORMAT_13K) {
        qcp_data.buffer       = (uint8 *) audqcp_13k_header;
        audqcp_rec_head_bytes = AUDQCP_HEADER_SIZE;
      }
      if (audqcp_rec_format == QCPSUP_FORMAT_AMR) {
        qcp_data.buffer       = (uint8 *) audqcp_amr_header;
        audqcp_rec_head_bytes = AUDQCP_HEADER_SIZE;
      }
      if (audqcp_rec_format == QCPSUP_FORMAT_EVRC) {
        qcp_data.buffer       = (uint8 *) audqcp_evrc_header;
        audqcp_rec_head_bytes = AUDQCP_HEADER_SIZE;
      }
      if (audqcp_rec_format == QCPSUP_FORMAT_4GV_NB) {
        qcp_data.buffer       = (uint8 *) audqcp_4gv_nb_header;
        audqcp_rec_head_bytes = AUDQCP_HEADER_SIZE;
      }
#ifdef FEATURE_AUDFMT_AMR
      if (audqcp_rec_format == QCPSUP_AMR_FILE_FORMAT) {
        /* Only support single channel recording of narrow-band AMR data */
        qcp_data.buffer       = (uint8 *) amr_id;
        audqcp_rec_head_bytes = AUDAMR_HEADER_SIZE(amr_id);
      }
#endif /* FEATURE_AUDFMT_AMR */
#ifdef FEATURE_AUDFMT_EVB
      if (audqcp_rec_format == QCPSUP_EVB_FILE_FORMAT) {
        qcp_data.buffer       = (uint8 *) evb_id;
        audqcp_rec_head_bytes = AUDEVB_HEADER_SIZE;
      }
#endif /* FEATURE_AUDFMT_EVB */

#ifdef FEATURE_AUDFMT_EVW
      if (audqcp_rec_format == QCPSUP_EVW_FILE_FORMAT) {
        qcp_data.buffer       = (uint8 *) evw_id;
        audqcp_rec_head_bytes = AUDEVW_HEADER_SIZE;
      }
#endif /* FEATURE_AUDFMT_EVW */

      if (audqcp_rec_format == QCPSUP_FORMAT_EFR) {
        qcp_data.buffer       = (uint8 *) audqcp_efr_header;
        audqcp_rec_head_bytes = AUDQCP_HEADER_SIZE;
      }
      if (audqcp_rec_format == QCPSUP_FORMAT_FR) {
        qcp_data.buffer       = (uint8 *) audqcp_fr_header;
        audqcp_rec_head_bytes = AUDQCP_HEADER_SIZE;
      }
      if (audqcp_rec_format == QCPSUP_FORMAT_HR) {
        qcp_data.buffer       = (uint8 *) audqcp_hr_header;
        audqcp_rec_head_bytes = AUDQCP_HEADER_SIZE;
      }
      qcp_data.position  = 0;
      qcp_data.num_bytes = audqcp_rec_head_bytes;
      qcp_data.content   = SND_QCP_REC_DATA_BUFFER_UNCATAGORIZED;
      if((link == SND_QCP_DIR_REVERSE) || (link == SND_QCP_DIR_BOTH)) {
        qcp_data.link  = SND_QCP_DIR_REVERSE;
        cb_func(SND_CMX_QCP_REC_DATA, &qcp_data, client_data);
      }
      if((link == SND_QCP_DIR_FORWARD) || (link == SND_QCP_DIR_BOTH)) {
        qcp_data.link  = SND_QCP_DIR_FORWARD;
        cb_func(SND_CMX_QCP_REC_DATA, &qcp_data, client_data);
      }
    }

    if((ret_val = audmain_get_buffer(cb_func, client_data)) ==
       AUDMAIN_STATUS_SUCCESS) {
      if(link == SND_QCP_DIR_BOTH) {
        ret_val = audmain_get_buffer(cb_func, client_data);
      }
    }

    if(ret_val == AUDMAIN_STATUS_SUCCESS) {
      audmain_set_read_sync(FALSE);
      ret_val = audmain_get_buffer(cb_func, client_data);
      if((link == SND_QCP_DIR_BOTH) && (ret_val == AUDMAIN_STATUS_SUCCESS)) {
        ret_val = audmain_get_buffer(cb_func, client_data);
      }
    }
  }
  MSG_ERROR("audqcp_record started %d",ret_val,0,0);
  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audqcp_record_stop

DESCRIPTION
  This function cleans up at the end of recording.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audqcp_record_stop ( 
  audmain_rec_status_type status
) {
  audmain_status_type ret_val = AUDMAIN_STATUS_FAILURE;
  snd_qcp_rec_data_struct_type qcp_data;
  static uint32 tx_file_size, tx_bytes, tx_frames;
  static uint32 rx_file_size, rx_bytes, rx_frames;

  if(audqcp_rec_cb != NULL) {
    /* Add various header bytes if recording QCP file */
#if defined(FEATURE_AUDFMT_AMR) || defined(FEATURE_AUDFMT_EVB) \
                                || defined(FEATURE_AUDFMT_EVW)
    if((audqcp_rec_format != QCPSUP_AMR_FILE_FORMAT) &&
       (audqcp_rec_format != QCPSUP_EVB_FILE_FORMAT) &&
       (audqcp_rec_format != QCPSUP_EVW_FILE_FORMAT))
#endif /* FEATURE_AUDFMT_AMR || FEATURE_AUDFMT_EVB || FEATURE_AUDFMT_EVW*/
    {
      if((audqcp_rec_link == SND_QCP_DIR_REVERSE) ||
         (audqcp_rec_link == SND_QCP_DIR_BOTH)) {

        tx_file_size       = audqcp_rec_tx_bytes + AUDQCP_HEADER_SIZE - 8;
        if(audqcp_rec_tx_bytes & 1) {
          tx_file_size++;
        }
        qcp_data.buffer    = (uint8 *) &tx_file_size;
        qcp_data.position  = 4;
        qcp_data.num_bytes = 4;
        qcp_data.link      = SND_QCP_DIR_REVERSE;
        qcp_data.content   = SND_QCP_REC_DATA_BUFFER_UNCATAGORIZED;
        audqcp_rec_cb(SND_CMX_QCP_REC_DATA, &qcp_data, audqcp_client_data);

        tx_bytes           = audqcp_rec_tx_bytes;
        qcp_data.buffer    = (uint8 *) &tx_bytes;
        qcp_data.position  = 190;
        qcp_data.num_bytes = 4;
        qcp_data.link      = SND_QCP_DIR_REVERSE;
        qcp_data.content   = SND_QCP_REC_DATA_BUFFER_UNCATAGORIZED;
        audqcp_rec_cb(SND_CMX_QCP_REC_DATA, &qcp_data, audqcp_client_data);

        tx_frames          = audqcp_rec_tx_frames;
        qcp_data.buffer    = (uint8 *) &tx_frames;
        qcp_data.position  = 182;
        qcp_data.num_bytes = 4;
        qcp_data.link      = SND_QCP_DIR_REVERSE;
        qcp_data.content   = SND_QCP_REC_DATA_BUFFER_UNCATAGORIZED;
        audqcp_rec_cb(SND_CMX_QCP_REC_DATA, &qcp_data, audqcp_client_data);

        if(audqcp_rec_tx_bytes & 1) {
          qcp_data.buffer    = (uint8 *) &audqcp_pad_byte;
          qcp_data.position  = 190 + 4 + audqcp_rec_tx_bytes;
          qcp_data.num_bytes = 1;
          qcp_data.link      = SND_QCP_DIR_REVERSE;
          qcp_data.content   = SND_QCP_REC_DATA_BUFFER_UNCATAGORIZED;
          audqcp_rec_cb(SND_CMX_QCP_REC_DATA, &qcp_data, audqcp_client_data);
        }
      }

      if((audqcp_rec_link == SND_QCP_DIR_FORWARD) ||
         (audqcp_rec_link == SND_QCP_DIR_BOTH)) {
        rx_file_size       = audqcp_rec_rx_bytes + AUDQCP_HEADER_SIZE - 8;
        if(audqcp_rec_rx_bytes & 1) {
          rx_file_size++;
        }
        qcp_data.buffer    = (uint8 *) &rx_file_size;
        qcp_data.position  = 4;
        qcp_data.num_bytes = 4;
        qcp_data.link      = SND_QCP_DIR_FORWARD;
        qcp_data.content   = SND_QCP_REC_DATA_BUFFER_UNCATAGORIZED;
        audqcp_rec_cb(SND_CMX_QCP_REC_DATA, &qcp_data, audqcp_client_data);

        rx_bytes           = audqcp_rec_rx_bytes;
        qcp_data.buffer    = (uint8 *) &rx_bytes;
        qcp_data.position  = 190;
        qcp_data.num_bytes = 4;
        qcp_data.link      = SND_QCP_DIR_FORWARD;
        qcp_data.content   = SND_QCP_REC_DATA_BUFFER_UNCATAGORIZED;
        audqcp_rec_cb(SND_CMX_QCP_REC_DATA, &qcp_data, audqcp_client_data);

        rx_frames          = audqcp_rec_rx_frames;
        qcp_data.buffer    = (uint8 *) &rx_frames;
        qcp_data.position  = 182;
        qcp_data.num_bytes = 4;
        qcp_data.link      = SND_QCP_DIR_FORWARD;
        qcp_data.content   = SND_QCP_REC_DATA_BUFFER_UNCATAGORIZED;
        audqcp_rec_cb(SND_CMX_QCP_REC_DATA, &qcp_data, audqcp_client_data);

        if(audqcp_rec_rx_bytes & 1) {
          qcp_data.buffer    = (uint8 *) &audqcp_pad_byte;
          qcp_data.position  = 190 + 4 + audqcp_rec_rx_bytes;
          qcp_data.num_bytes = 1;
          qcp_data.link      = SND_QCP_DIR_FORWARD;
          qcp_data.content   = SND_QCP_REC_DATA_BUFFER_UNCATAGORIZED;
          audqcp_rec_cb(SND_CMX_QCP_REC_DATA, &qcp_data, audqcp_client_data);
        }
      }
    }

    /* When stopping recording, return status to originator */
    if(status == AUDMAIN_REC_BUFF_ERROR) {
      audqcp_rec_cb(SND_CMX_QCP_REC_BUFF_ERR, NULL, audqcp_client_data);
    } else if(status == AUDMAIN_REC_ERROR) {
      audqcp_rec_cb(SND_CMX_QCP_REC_ERROR, NULL, audqcp_client_data);
    } else if(status == AUDMAIN_REC_AUTO_STOP) {
      audqcp_rec_cb(SND_CMX_QCP_REC_AUTO_STOP, NULL, audqcp_client_data);
    } else {
      audqcp_rec_cb(SND_CMX_QCP_REC_STOP, NULL, audqcp_client_data);
    }
    audqcp_rec_cb = NULL;
    ret_val = AUDMAIN_STATUS_SUCCESS;
  }
  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audqcp_rec_voc_cb

DESCRIPTION
  This function handles recording callbacks from the vocoder driver.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audqcp_rec_voc_cb (
  audmain_rec_status_type status,
  uint8                   *buffer,
  uint32                  num_bytes,
  uint32                  num_frames
) {
  snd_qcp_rec_data_struct_type qcp_data;

  if(audqcp_rec_cb != NULL) {
    switch (status) {
      case AUDMAIN_REC_RX_BUFFER:
      case AUDMAIN_REC_LAST_RX_BUFFER:
        qcp_data.buffer    = buffer;
        qcp_data.position  = audqcp_rec_head_bytes + audqcp_rec_rx_bytes;
        qcp_data.num_bytes = num_bytes;
        qcp_data.link      = SND_QCP_DIR_FORWARD;
        qcp_data.content   = SND_QCP_REC_DATA_BUFFER_RECDATA;

        audqcp_rec_rx_bytes  += num_bytes;
        audqcp_rec_rx_frames += num_frames;

        audqcp_rec_cb(SND_CMX_QCP_REC_DATA, &qcp_data, audqcp_client_data);

        if(status != AUDMAIN_REC_LAST_RX_BUFFER) {
          (void) audmain_get_buffer(audqcp_rec_cb, audqcp_client_data);
        }
        break;

      case AUDMAIN_REC_TX_BUFFER:
      case AUDMAIN_REC_LAST_TX_BUFFER:
        qcp_data.buffer    = buffer;
        qcp_data.position  = audqcp_rec_head_bytes + audqcp_rec_tx_bytes;
        qcp_data.num_bytes = num_bytes;
        qcp_data.link      = SND_QCP_DIR_REVERSE;
        qcp_data.content   = SND_QCP_REC_DATA_BUFFER_RECDATA;

        audqcp_rec_tx_bytes  += num_bytes;
        audqcp_rec_tx_frames += num_frames;

        audqcp_rec_cb(SND_CMX_QCP_REC_DATA, &qcp_data, audqcp_client_data);

        if(status != AUDMAIN_REC_LAST_TX_BUFFER) {
          (void) audmain_get_buffer(audqcp_rec_cb, audqcp_client_data);
        }
        break;

      case AUDMAIN_REC_INFORM_13K:
        if((audqcp_rec_rx_frames == 0) && (audqcp_rec_tx_frames == 0)) {
          audqcp_rec_format  = QCPSUP_FORMAT_13K;
          qcp_data.buffer    = (uint8 *) audqcp_13k_header;
          qcp_data.position  = 0;
          qcp_data.num_bytes = AUDQCP_HEADER_SIZE;
          qcp_data.content   = SND_QCP_REC_DATA_BUFFER_UNCATAGORIZED;
          audqcp_rec_head_bytes = AUDQCP_HEADER_SIZE;		

          if((audqcp_rec_link == SND_QCP_DIR_REVERSE) ||
             (audqcp_rec_link == SND_QCP_DIR_BOTH)) {
            qcp_data.link  = SND_QCP_DIR_REVERSE;
            audqcp_rec_cb(SND_CMX_QCP_REC_DATA, &qcp_data, audqcp_client_data);
          }
          if((audqcp_rec_link == SND_QCP_DIR_FORWARD) ||
             (audqcp_rec_link == SND_QCP_DIR_BOTH)) {
            qcp_data.link  = SND_QCP_DIR_FORWARD;
            audqcp_rec_cb(SND_CMX_QCP_REC_DATA, &qcp_data, audqcp_client_data);
          }
        } else {
          (void) audqcp_record_stop(AUDMAIN_REC_ERROR);
        }
        break;

      case AUDMAIN_REC_INFORM_EVRC:
        if((audqcp_rec_rx_frames == 0) && (audqcp_rec_tx_frames == 0)) {
          audqcp_rec_format  = QCPSUP_FORMAT_EVRC;
          qcp_data.buffer    = (uint8 *) audqcp_evrc_header;
          qcp_data.position  = 0;
          qcp_data.num_bytes = AUDQCP_HEADER_SIZE;
          qcp_data.content   = SND_QCP_REC_DATA_BUFFER_UNCATAGORIZED;
          audqcp_rec_head_bytes = AUDQCP_HEADER_SIZE;		
          if((audqcp_rec_link == SND_QCP_DIR_REVERSE) ||
             (audqcp_rec_link == SND_QCP_DIR_BOTH)) {
            qcp_data.link  = SND_QCP_DIR_REVERSE;
            audqcp_rec_cb(SND_CMX_QCP_REC_DATA, &qcp_data, audqcp_client_data);
          }
          if((audqcp_rec_link == SND_QCP_DIR_FORWARD) ||
             (audqcp_rec_link == SND_QCP_DIR_BOTH)) {
            qcp_data.link  = SND_QCP_DIR_FORWARD;
            audqcp_rec_cb(SND_CMX_QCP_REC_DATA, &qcp_data, audqcp_client_data);
          }
        } else {
          (void) audqcp_record_stop(AUDMAIN_REC_ERROR);
        }
        break;

      case AUDMAIN_REC_INFORM_4GV_NB:
        if((audqcp_rec_rx_frames == 0) && (audqcp_rec_tx_frames == 0)) {
          audqcp_rec_format  = QCPSUP_FORMAT_4GV_NB;
          qcp_data.buffer    = (uint8 *) audqcp_4gv_nb_header;
          qcp_data.position  = 0;
          qcp_data.num_bytes = AUDQCP_HEADER_SIZE;
          qcp_data.content   = SND_QCP_REC_DATA_BUFFER_UNCATAGORIZED;
          audqcp_rec_head_bytes = AUDQCP_HEADER_SIZE;

          if((audqcp_rec_link == SND_QCP_DIR_REVERSE) ||
             (audqcp_rec_link == SND_QCP_DIR_BOTH)) {
            qcp_data.link  = SND_QCP_DIR_REVERSE;
            audqcp_rec_cb(SND_CMX_QCP_REC_DATA, &qcp_data, audqcp_client_data);
          }
          if((audqcp_rec_link == SND_QCP_DIR_FORWARD) ||
             (audqcp_rec_link == SND_QCP_DIR_BOTH)) {
            qcp_data.link  = SND_QCP_DIR_FORWARD;
            audqcp_rec_cb(SND_CMX_QCP_REC_DATA, &qcp_data, audqcp_client_data);
          }
        } else {
          (void) audqcp_record_stop(AUDMAIN_REC_ERROR);
        }
        break;

      case AUDMAIN_REC_INFORM_AMR:
        if((audqcp_rec_rx_frames == 0) && (audqcp_rec_tx_frames == 0)) {
          audqcp_rec_format  = QCPSUP_FORMAT_AMR;
          qcp_data.buffer    = (uint8 *) audqcp_amr_header;
          qcp_data.position  = 0;
          qcp_data.num_bytes = AUDQCP_HEADER_SIZE;
          qcp_data.content   = SND_QCP_REC_DATA_BUFFER_UNCATAGORIZED;

          if((audqcp_rec_link == SND_QCP_DIR_REVERSE) ||
             (audqcp_rec_link == SND_QCP_DIR_BOTH)) {
            qcp_data.link  = SND_QCP_DIR_REVERSE;
            audqcp_rec_cb(SND_CMX_QCP_REC_DATA, &qcp_data, audqcp_client_data);
          }
          if((audqcp_rec_link == SND_QCP_DIR_FORWARD) ||
             (audqcp_rec_link == SND_QCP_DIR_BOTH)) {
            qcp_data.link  = SND_QCP_DIR_FORWARD;
            audqcp_rec_cb(SND_CMX_QCP_REC_DATA, &qcp_data, audqcp_client_data);
          }
        } else {
          (void) audqcp_record_stop(AUDMAIN_REC_ERROR);
        }
        break;

      case AUDMAIN_REC_INFORM_EFR:
        if((audqcp_rec_rx_frames == 0) && (audqcp_rec_tx_frames == 0)) {
          audqcp_rec_format  = QCPSUP_FORMAT_EFR;
          qcp_data.buffer    = (uint8 *) audqcp_efr_header;
          qcp_data.position  = 0;
          qcp_data.num_bytes = AUDQCP_HEADER_SIZE;
          qcp_data.content   = SND_QCP_REC_DATA_BUFFER_UNCATAGORIZED;
          audqcp_rec_head_bytes = AUDQCP_HEADER_SIZE;     
          if((audqcp_rec_link == SND_QCP_DIR_REVERSE) ||
             (audqcp_rec_link == SND_QCP_DIR_BOTH)) {
            qcp_data.link  = SND_QCP_DIR_REVERSE;
            audqcp_rec_cb(SND_CMX_QCP_REC_DATA, &qcp_data, audqcp_client_data);
          }
          if((audqcp_rec_link == SND_QCP_DIR_FORWARD) ||
             (audqcp_rec_link == SND_QCP_DIR_BOTH)) {
            qcp_data.link  = SND_QCP_DIR_FORWARD;
            audqcp_rec_cb(SND_CMX_QCP_REC_DATA, &qcp_data, audqcp_client_data);
          }
        } else {
          (void) audqcp_record_stop(AUDMAIN_REC_ERROR);
        }
        break;

      case AUDMAIN_REC_INFORM_FR:
        if((audqcp_rec_rx_frames == 0) && (audqcp_rec_tx_frames == 0)) {
          audqcp_rec_format  = QCPSUP_FORMAT_FR;
          qcp_data.buffer    = (uint8 *) audqcp_fr_header;
          qcp_data.position  = 0;
          qcp_data.num_bytes = AUDQCP_HEADER_SIZE;
          qcp_data.content   = SND_QCP_REC_DATA_BUFFER_UNCATAGORIZED;
          audqcp_rec_head_bytes = AUDQCP_HEADER_SIZE;
          if((audqcp_rec_link == SND_QCP_DIR_REVERSE) ||
             (audqcp_rec_link == SND_QCP_DIR_BOTH)) {
            qcp_data.link  = SND_QCP_DIR_REVERSE;
            audqcp_rec_cb(SND_CMX_QCP_REC_DATA, &qcp_data, audqcp_client_data);
          }
          if((audqcp_rec_link == SND_QCP_DIR_FORWARD) ||
             (audqcp_rec_link == SND_QCP_DIR_BOTH)) {
            qcp_data.link  = SND_QCP_DIR_FORWARD;
            audqcp_rec_cb(SND_CMX_QCP_REC_DATA, &qcp_data, audqcp_client_data);
          }
        } else {
          (void) audqcp_record_stop(AUDMAIN_REC_ERROR);
        }
        break;

      case AUDMAIN_REC_INFORM_HR:
        if((audqcp_rec_rx_frames == 0) && (audqcp_rec_tx_frames == 0)) {
          audqcp_rec_format  = QCPSUP_FORMAT_HR;
          qcp_data.buffer    = (uint8 *) audqcp_hr_header;
          qcp_data.position  = 0;
          qcp_data.num_bytes = AUDQCP_HEADER_SIZE;
          qcp_data.content   = SND_QCP_REC_DATA_BUFFER_UNCATAGORIZED;
          audqcp_rec_head_bytes = AUDQCP_HEADER_SIZE;					
          if((audqcp_rec_link == SND_QCP_DIR_REVERSE) ||
             (audqcp_rec_link == SND_QCP_DIR_BOTH)) {
            qcp_data.link  = SND_QCP_DIR_REVERSE;
            audqcp_rec_cb(SND_CMX_QCP_REC_DATA, &qcp_data, audqcp_client_data);
          }
          if((audqcp_rec_link == SND_QCP_DIR_FORWARD) ||
             (audqcp_rec_link == SND_QCP_DIR_BOTH)) {
            qcp_data.link  = SND_QCP_DIR_FORWARD;
            audqcp_rec_cb(SND_CMX_QCP_REC_DATA, &qcp_data, audqcp_client_data);
          }
        } else {
          (void) audqcp_record_stop(AUDMAIN_REC_ERROR);
        }
        break;

      case AUDMAIN_REC_CHANGE_CONFIG:
        (void) audqcp_record_stop(AUDMAIN_REC_ERROR);
        break;

      case AUDMAIN_REC_PAUSED:
        audqcp_rec_cb(SND_CMX_AF_PAUSE, NULL, audqcp_client_data);
        break;

      case AUDMAIN_REC_RESUMED:
        audqcp_rec_cb(SND_CMX_AF_RESUME, NULL, audqcp_client_data);
        break;

      case AUDMAIN_REC_INT_TIME:
        audqcp_rec_cb(SND_CMX_QCP_INT_TIME, NULL, audqcp_client_data);
        break;

      default:
        break;
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION audqcp_do_time

DESCRIPTION
  This function gets the total length of file in ms.

DEPENDENCIES
  audqcp_parse_ctl

RETURN VALUE
  AUDMAIN_STATUS_DONE - Success

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audqcp_do_time (
  audmain_parse_ctl_type  *parse_ctl,
  uint32                  *elapsed_ms
) {  

  audmain_status_type result;
  audqcp_prompt_type *curr_prompt, *other_prompt;

  if ((audqcp_parse_ctl.status_flag & AUDQCP_MASK_FAILURE) ==
      AUDQCP_MASK_FAILURE) {
    return AUDMAIN_STATUS_ERROR;
  } else if (audqcp_parse_ctl.status == AUDQCP_STATUS_PLAY) {
    *elapsed_ms = audqcp_parse_ctl.num_frames * AUDQCP_FRAME_TIME;
    return AUDMAIN_STATUS_DONE;
  } else if( audqcp_parse_ctl.status == AUDQCP_STATUS_NONE) {
    return AUDMAIN_STATUS_ERROR;
  } else if( audqcp_parse_ctl.status == AUDQCP_STATUS_PARSE_HEAD &&
    audqcp_parse_ctl.num_frames != 0 ) {
    *elapsed_ms = audqcp_parse_ctl.num_frames * AUDQCP_FRAME_TIME;
    audqcp_parse_ctl.status = AUDQCP_STATUS_DONE_TIME;
    return AUDMAIN_STATUS_DONE;
  } else {

    curr_prompt = audqcp_parse_ctl.curr_prompt;

    if(curr_prompt == &audqcp_parse_ctl.prompt1) {
      other_prompt = &audqcp_parse_ctl.prompt2;
    } else if(curr_prompt == &audqcp_parse_ctl.prompt2) {
      other_prompt = &audqcp_parse_ctl.prompt1;
    } else if(audqcp_buf_mgmt.first == &audqcp_parse_ctl.prompt1 ||
      audqcp_buf_mgmt.first == &audqcp_parse_ctl.prompt2 ){
      audqcp_parse_ctl.status = AUDQCP_STATUS_CALC_TIME;
      return AUDMAIN_STATUS_PENDING;
    } else {
      return AUDMAIN_STATUS_ERROR;
    }

    audqcp_parse_ctl.num_frames += (curr_prompt->prompt.pb_num_frames +
                                    curr_prompt->boarder.pb_num_frames);
    if(other_prompt->track_buf != NULL) {
      audqcp_parse_ctl.num_frames += (other_prompt->prompt.pb_num_frames +
                                      other_prompt->boarder.pb_num_frames);


      result = audqcp_get_data( (audqcp_parse_ctl_type*)parse_ctl->tracks, 
                 &parse_ctl->current_buf, other_prompt);
      if(result == AUDMAIN_STATUS_DONE) {
        *elapsed_ms = audqcp_parse_ctl.num_frames * AUDQCP_FRAME_TIME;
        audqcp_parse_ctl.status = AUDQCP_STATUS_DONE_TIME;
        return AUDMAIN_STATUS_DONE;
      } else if(result == AUDMAIN_STATUS_FAILURE) {
        audqcp_parse_ctl.status = AUDQCP_STATUS_DONE_TIME;
        return AUDMAIN_STATUS_ERROR;
      }
    }

    if(audqcp_buf_mgmt.first != curr_prompt) {
      result = audqcp_get_data((audqcp_parse_ctl_type*)parse_ctl->tracks, 
                               &parse_ctl->current_buf, curr_prompt);
      if(result == AUDMAIN_STATUS_DONE) {
        *elapsed_ms = audqcp_parse_ctl.num_frames * AUDQCP_FRAME_TIME;
        audqcp_parse_ctl.status = AUDQCP_STATUS_DONE_TIME;
        return AUDMAIN_STATUS_DONE;
      } else if(result == AUDMAIN_STATUS_FAILURE) {
        audqcp_parse_ctl.status = AUDQCP_STATUS_DONE_TIME;
        return AUDMAIN_STATUS_ERROR;
      }
    }

    audqcp_parse_ctl.curr_prompt = other_prompt;
    audqcp_parse_ctl.status = AUDQCP_STATUS_CALC_TIME;
    return AUDMAIN_STATUS_PENDING;

  }

  return AUDMAIN_STATUS_ERROR;
}
#endif /* FEATURE_QCP */

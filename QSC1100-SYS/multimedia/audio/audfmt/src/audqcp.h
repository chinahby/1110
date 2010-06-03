#ifndef AUDQCP_H
#define AUDQCP_H
/*===========================================================================

         Q C P   A U D I O   F O R M A T S    H E A D E R    F I L E

DESCRIPTION
  This header file contains all the definitions necessary for other
  tasks to interface with the QCP audio formats module.

Copyright (c) 2001 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.
   
  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/audfmt/main/latest/src/audqcp.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/22/04    lg     Added get_time functionality
07/25/02    st     Added audqcp_format_play() to play files with no headers
                   and audqcp_flush().
09/16/01    st     Modified AUDQCP_FUNC_GET_CNFG to AUDQCP_FUNC_CNFG in order
                   to use for setting cnfg chunk.
09/03/01  st/aw    Added code to support ff/rewind during QCP playback; added
                   code to support QCP recording; added code to support QCP
                   support functions (get/set chunk information).
04/04/01    st     created file

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"             /* Target specific definitions            */
#include "comdef.h"             /* Definitions for byte, word, etc.       */

#ifdef FEATURE_QCP

#include "snd.h"                /* Sound typedefs and prototypes          */
#include "audmaini.h"           /* Audio format typedefs and prototypes   */

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/* The following enumerated type is used to indicate the type of function
** to execute.
*/
typedef enum {
  AUDQCP_FUNC_GET_DATA,   /* Get size of data chunk in QCP file */
  AUDQCP_FUNC_GET_VRAT,   /* Get number of frames in QCP file   */
  AUDQCP_FUNC_CNFG,       /* Process cnfg chunk of QCP file     */
  AUDQCP_FUNC_LABL,       /* Process labl chunk of QCP file     */
  AUDQCP_FUNC_TEXT,       /* Process text chunk of QCP file     */
  AUDQCP_FUNC_INVALID
} audqcp_func_type;

/* <EJECT> */
/*---------------------------------------------------------------------------
** AUDQCP Public Function Prototypes
**---------------------------------------------------------------------------
*/
/*===========================================================================

FUNCTION audqcp_func_get_data_cb_req

DESCRIPTION
  This function handles the get data callback for QCP function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean audqcp_func_get_data_cb_req (
  uint8 **data,
  uint8 *buf,
  uint32 length,
  uint32 pos
);

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
extern boolean audqcp_get_data_cb_req(
  uint8  **data,
  uint8  *buf,
  uint32 length,
  uint32 pos
);

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
extern audmain_status_type audqcp_format_play (
  snd_cmx_format_spec_type *input_spec,
  uint8                    *buffer,
  snd_af_access_type       *handle,
  audmain_parse_ctl_type   *parse_ctl
);

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
extern audmain_status_type audqcp_parse_head (
  uint8                       *buffer,
  snd_af_access_type          *handle,
  snd_audfmt_start_enum_type  start_position,
  audmain_parse_ctl_type      *parse_ctl
);

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
extern void audqcp_age ( 
  audmain_parse_ctl_type  *parse_ctl,
  uint32                  *elapsed_time,
  uint32                  *adjusted_time 
);

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
extern audmain_status_type audqcp_parse (
  audmain_parse_ctl_type *parse_ctl,
  audmain_cmd_buf_type   *cmd_buf,
  uint32                 *time_remain
);

/* <EJECT> */
/*===========================================================================

FUNCTION audqcp_fforward

DESCRIPTION
  This function moves the playback forward ms milliseconds.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern audmain_status_type audqcp_fforward (
  audmain_parse_ctl_type *parse_ctl,
  uint32                 ms_forward,
  uint32                 *elapsed_ms,
  uint32                 *adjusted_ms
);

/* <EJECT> */
/*===========================================================================

FUNCTION audqcp_rewind

DESCRIPTION
  This function moves the playback back ms milliseconds.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern audmain_status_type audqcp_rewind (
  audmain_parse_ctl_type *parse_ctl,
  uint32                 ms_rewind,
  uint32                 *elapsed_ms,
  uint32                 *adjusted_ms
);

/* <EJECT> */
/*===========================================================================

FUNCTION audqcp_do_seek

DESCRIPTION
  This function moves the playback to the proper position.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern audmain_status_type audqcp_do_seek (
  audmain_parse_ctl_type *parse_ctl,
  audmain_cmd_buf_type   *cmd_buf,
  uint32                 *elapsed_ms,
  uint32                 *adjusted_ms
);

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
extern audmain_status_type audqcp_flush (
  audmain_parse_ctl_type *parse_ctl
);

/* <EJECT> */
/*===========================================================================

FUNCTION audqcp_get_func

DESCRIPTION
  This function finds the vrat or data or cnfg chunk in a QCP file and
  returns the relavent information.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void audqcp_get_func (
  audqcp_func_type       func,
  snd_af_access_type     *handle,
  audmain_parse_ctl_type *parse_ctl
);

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
extern void audqcp_get_chunk_data (
  audqcp_func_type       func,
  snd_af_access_type     *handle,
  audmain_parse_ctl_type *parse_ctl
);

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
extern void audqcp_set_chunk_data (
  audqcp_func_type               func,
  audmain_parse_ctl_type         *parse_ctl,
  uint8                          *buffer,
  uint32                         num_bytes,
  snd_af_access_type             *handle,
  snd_qcp_rec_cb_func_ptr_type   cb_func,
  const void                     *client_data
);

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
extern audmain_status_type audqcp_record (
  snd_qcp_format_type          format,
  snd_qcp_dir_type             link,
  snd_qcp_rec_cb_func_ptr_type cb_func,
  const void                   *client_data
);

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
extern audmain_status_type audqcp_record_stop ( 
  audmain_rec_status_type status
);

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
extern void audqcp_rec_voc_cb (
  audmain_rec_status_type status,
  uint8                   *buffer,
  uint32                  num_bytes,
  uint32                  num_frames
);

/* <EJECT> */
/*===========================================================================

FUNCTION audqcp_do_time

DESCRIPTION
  This function gets the total file length in ms.

DEPENDENCIES
  audqcp_parse_ctl

RETURN VALUE
  AUDMAIN_STATUS_DONE - Success

SIDE EFFECTS
  None

===========================================================================*/
extern audmain_status_type audqcp_do_time (
  audmain_parse_ctl_type  *parse_ctl,
  uint32                  *elapsed_ms
);

#endif /* FEATURE_QCP */

#endif /* AUDQCP_H */

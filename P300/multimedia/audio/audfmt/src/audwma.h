#ifndef AUDWMA_H
#define AUDWMA_H
/*===========================================================================

     W I N D O W S   M E D I A   A U D I O   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface
  WindowsMedia Audio streams with the Audio Format modules.

Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
   
  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/audfmt/main/latest/src/audwma.h#1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/14/05    hz     Initial version to check in code base
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"     /* Target specific definitions                     */
#include "customer.h"   /* Customer specific definitions                   */
#include "comdef.h"     /* Definitions for byte, word, etc.                */

#ifdef FEATURE_WMA
#include "audmaini.h"   /* Audio Format Data definitions and protos        */
#include "snd.h"

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/


/* <EJECT> */
/*---------------------------------------------------------------------------
** audwma Public Function Prototypes
**--------------------------------------------------------------------------*/

/* <EJECT> */
/*===========================================================================

FUNCTION audwma_format_play

DESCRIPTION
  Use this function to play streaming WindowsMedia Audio.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_SUCCESS if successful

SIDE EFFECTS

===========================================================================*/
extern audmain_status_type audwma_format_play (
  snd_wma_format_spec_type    *input_spec, /* Stream format info      */
  uint8                       *buffer,     /* CMX Input buffer        */
  snd_af_access_type          *handle,     /* CMX Data handle         */
  audmain_parse_ctl_type      *parse_ctl   /* data buffer info        */
);

/* <EJECT> */
/*===========================================================================

FUNCTION audwma_get_data_cb_req

DESCRIPTION
  Callback function which handles the response to a Data request.

DEPENDENCIES
  audfmt_buf_mgmt

RETURN VALUE
  TRUE if the data was successfully obtained

SIDE EFFECTS
  audfmt_buf_mgmt

===========================================================================*/
extern boolean audwma_get_data_cb_req (
  uint8  **data, 
  uint8  *buf, 
  uint32 length,
  uint32 pos
);

/* <EJECT> */
/*===========================================================================

FUNCTION audwma_get_sample_rate

DESCRIPTION
  This function returns the sampling rate of the current song.

DEPENDENCIES

RETURN VALUE
  sample rate of the last file parsed

SIDE EFFECTS
  None

===========================================================================*/
extern snd_sample_rate_type audwma_get_sample_rate (
  audmain_parse_ctl_type  *parse_ctl
);

/* <EJECT> */
/*===========================================================================

FUNCTION audwma_flush

DESCRIPTION
  This function flushes the buffers and restarts playback.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern audmain_status_type audwma_flush (
  audmain_parse_ctl_type   *parse_ctl
);

/* <EJECT> */
/*===========================================================================

FUNCTION audwma_reset

DESCRIPTION
  This function resets internal WindowsMedia Audio buffers after a song 
  is unloaded.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  ra_info

===========================================================================*/
extern void audwma_reset ( void );

/* <EJECT> */
/*===========================================================================

FUNCTION audwma_adec_adjust

DESCRIPTION
  This function adjusts adec buffers to reflect their newly used data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern audmain_status_type audwma_adec_adjust (
  audmain_parse_ctl_type  *parse_ctl,
  uint32                  *bytes_used
);

/* <EJECT> */
/*===========================================================================

FUNCTION audwma_parse

DESCRIPTION
  This function parses one event of the WindowsMedia Audio file. Prepares 
  buffers for data transfer.

DEPENDENCIES
  Must call audwma_age() before calling this function!

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern audmain_status_type audwma_parse (
  audmain_parse_ctl_type  *parse_ctl,
  audmain_cmd_buf_type    *cmd_buf,
  uint32                  *time_remain
);

/* <EJECT> */
/*===========================================================================

FUNCTION audwma_age

DESCRIPTION
  This function checks to see if the buffers are ready for use.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void audwma_age (
  audmain_parse_ctl_type  *parse_ctl,
  uint32                  *elapsed_time,
  uint32                  *adjusted_time
);

#endif /* FEATURE_WMA */
#endif /* AUDWMA_H */


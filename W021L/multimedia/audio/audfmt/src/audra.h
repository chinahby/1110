#ifndef AUDRA_H
#define AUDRA_H
/*===========================================================================

           R E A L A U D I O   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface
  RealAudio streams with the Audio Format modules.

Copyright (c) 2003 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
   
  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/audfmt/main/latest/src/audra.h#1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/23/03    sg     Initial version
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"     /* Target specific definitions                     */
#include "customer.h"   /* Customer specific definitions                   */
#include "comdef.h"     /* Definitions for byte, word, etc.                */

#ifdef FEATURE_REAL_AUDIO
#include "audmaini.h"   /* Audio Format Data definitions and protos        */
#include "snd.h"

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/


/* <EJECT> */
/*---------------------------------------------------------------------------
** audra Public Function Prototypes
**---------------------------------------------------------------------------
*/
/* <EJECT> */
/*===========================================================================

FUNCTION audra_format_play

DESCRIPTION
  Use this function to play streaming RealAudio.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_SUCCESS if successful

SIDE EFFECTS

===========================================================================*/
extern audmain_status_type audra_format_play (
  snd_ra_format_spec_type     *input_spec, /* Stream format info           */
  uint8                       *buffer,     /* CMX Input buffer             */
  snd_af_access_type          *handle,     /* CMX Data handle              */
  audmain_parse_ctl_type      *parse_ctl   /* data buffer info             */
);

/* <EJECT> */
/*===========================================================================

FUNCTION audra_get_data_cb_req

DESCRIPTION
  Callback function which handles the response to a Data request.

DEPENDENCIES
  audfmt_buf_mgmt

RETURN VALUE
  TRUE if the data was successfully obtained

SIDE EFFECTS
  audfmt_buf_mgmt

===========================================================================*/
extern boolean audra_get_data_cb_req (
  uint8  **data, 
  uint8  *buf, 
  uint32 length,
  uint32 pos
);

/* <EJECT> */
/*===========================================================================

FUNCTION audra_get_sample_rate

DESCRIPTION
  This function returns the sampling rate of the current song.

DEPENDENCIES

RETURN VALUE
  sample rate of the last file parsed

SIDE EFFECTS
  None

===========================================================================*/
extern snd_sample_rate_type audra_get_sample_rate (
  audmain_parse_ctl_type  *parse_ctl
);

/* <EJECT> */
/*===========================================================================

FUNCTION audra_flush

DESCRIPTION
  This function flushes the buffers and restarts playback.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern audmain_status_type audra_flush (
  audmain_parse_ctl_type   *parse_ctl
);

/* <EJECT> */
/*===========================================================================

FUNCTION audra_reset

DESCRIPTION
  This function resets internal RealAudio buffers after a song is unloaded.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  ra_info

===========================================================================*/
extern void audra_reset ( void );

/* <EJECT> */
/*===========================================================================

FUNCTION audra_adec_adjust

DESCRIPTION
  This function adjusts adec buffers to reflect their newly used data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern audmain_status_type audra_adec_adjust (
  audmain_parse_ctl_type  *parse_ctl,
  uint32                  *bytes_used
);

/* <EJECT> */
/*===========================================================================

FUNCTION audra_parse

DESCRIPTION
  This function parses one event of the RealAudio file. Prepares buffers for
  data transfer.

DEPENDENCIES
  Must call audra_age() before calling this function!

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern audmain_status_type audra_parse (
  audmain_parse_ctl_type  *parse_ctl,
  audmain_cmd_buf_type    *cmd_buf,
  uint32                  *time_remain
);

/* <EJECT> */
/*===========================================================================

FUNCTION audra_age

DESCRIPTION
  This function checks to see if the buffers are ready for use.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void audra_age (
  audmain_parse_ctl_type  *parse_ctl,
  uint32                  *elapsed_time,
  uint32                  *adjusted_time
);

#endif /* FEATURE_REAL_AUDIO */
#endif /* AUDRA_H */


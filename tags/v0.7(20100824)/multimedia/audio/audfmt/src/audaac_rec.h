// $Header: //source/qcom/qct/multimedia/audio/6k/ver1/audfmt/main/latest/src/audaac_rec.h#1 $ $DateTime: 2008/04/24 13:56:20 $ $Author: yuanz $
#ifndef AUDFAAC_REC_H
#define AUDFAAC_REC_H
/*===========================================================================

   A U D I O   F O R M A T   A A C   R E C O D E R   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface AAC
  RECODER files with the Audio Format modules.

Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
   
  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/audfmt/main/latest/src/audaac_rec.h#1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/20/05    hz     Optimized AAC record interface.
03/08/05    hz     First revision for release.
===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"     /* Target specific definitions                     */
#include "customer.h"   /* Customer specific definitions                   */
#include "comdef.h"     /* Definitions for byte, word, etc.                */

#ifdef FEATURE_AAC_REC
#include "audmaini.h"   /* Audio Format Data definitions and protos        */
#include "snd.h"

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
** AudAAC Public Function Prototypes
**---------------------------------------------------------------------------
*/

/*===========================================================================

FUNCTION audaac_rec_stop

DESCRIPTION
  This function cleans up at the end of recording.

DEPENDENCIES
  FEATURE_AAC_REC

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern audmain_status_type audaac_rec_stop (void);

/*===========================================================================

FUNCTION audaac_rec_buf

DESCRIPTION
  This function accepts a buffer for AAC recording.

DEPENDENCIES
  FEATURE_AAC_REC

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void audaac_rec_buf (
  uint8       *buffer,
  uint32      num_bytes,
  const void  *client_data
);


/*===========================================================================

FUNCTION audaac_rec_voc_cb

DESCRIPTION
  This function handles recording callbacks from the vocoder driver.

DEPENDENCIES
  FEATURE_AAC_REC

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void audaac_rec_voc_cb (
  audmain_rec_status_type status,
  uint8                   *buffer,
  uint32                  num_bytes,
  uint32                  num_frames
);


/*===========================================================================

FUNCTION audaac_rec_start

DESCRIPTION
  Start a AAC record session.

DEPENDENCIES
  FEATURE_AAC_REC

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean audaac_rec_start(
  snd_mm_rec_aac_para_type    *rec_param,   /* Record parameters              */
  snd_mm_rec_cmd_struct_type  *buf,         /* MM record buffer info          */
  snd_mm_rec_cb_func_ptr_type cb_func,      /* Callback function to CMX       */
  const void                  *client_data  /* Client data pointer            */
);

/*===========================================================================

FUNCTION audaac_rec_pause

DESCRIPTION
  This function pauses/resumes a AAC recording session.

DEPENDENCIES
  FEATURE_AAC_REC

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audaac_rec_pause (
  boolean               pause
);

/*===========================================================================

FUNCTION audaac_rec_send_status

DESCRIPTION
  This function sends the indicated status back to the client.

DEPENDENCIES
  FEATURE_AAC_REC

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audaac_rec_send_status (
  snd_af_data_cmd_enum status
);

/*===========================================================================

FUNCTION audaac_rec_get_buf

DESCRIPTION
  This function will get the availabe buffer for encoded data.

DEPENDENCIES
  FEATURE_AAC_REC

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean audaac_rec_get_buf (
  voc_aac_record_buf_type *buf_type
);

/*===========================================================================

FUNCTION audaac_rec_frame

DESCRIPTION
  This function records aac encoded frame(s) of data into buffer(s).

DEPENDENCIES
  FEATURE_AAC_REC

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audaac_rec_frame (
  voc_aac_record_data_type *frame_info
);
#endif /* FEATURE_AAC_REC */
#endif /* AUDFAAC_REC_H */

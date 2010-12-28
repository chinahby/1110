/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                     AUDIO FEATURES
GENERAL DESCRIPTION
  Implementation of QConcert Plus Reverb
EXTERNALIZED FUNCTIONS
 
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright(c) 1992 - 2008 by QUALCOMM, Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
   
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/08/09   yp     Intial version containing basic implementation of Qconcert
                  plus reverb stub function for 6k code.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "audqconcert.h"
#include "snd.h"

/*===========================================================================

                     EXTERNAL API definitions

===========================================================================*/

/*===========================================================================
FUNCTION snd_set_qconcert_plus_reverb_params 

DESCRIPTION
  Configures the parameters for the Qconcert + reverb 3D audio expander.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
extern void snd_set_qconcert_plus_reverb_params (
  auddef_qconcert_plus_reverb_config_cmd_params_type *params,
    /* Parameters value for Qconcert + reverb effect */
  snd_cb_func_ptr_type     callback_ptr,
    /* Callback to indicate whether this ctl command was accepted */
  const void               *client_data
    /* Pointer to Client provided data */
) {
  /* qconcert plus reverb is not supported*/
      if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
}


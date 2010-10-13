#ifndef AUDQCONCERT_H
#define AUDQCONCERT_H
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

#include "snd.h"
#include "comdef.h"     /* Definitions for byte, word, etc.       */
#include "target.h"     /* Target Specific Defintions             */
#include "msg.h"        /* Message logging/reporting functions    */
#include "err.h"        /* Error logging/reporting services       */


/*CMD ID - to be used by UI to set qconcert + reverb params*/
typedef enum {
  AUDDEF_CMD_INVALID,
  AUDDEF_CMD_SET_PRESET,
  AUDDEF_CMD_SET_OUTPUT_MODE,   
  AUDDEF_CMD_SET_QCPR_MAX_NUM_OF_LEVELS,
  AUDDEF_CMD_SET_QCPR_CURRENT_LEVEL,
  AUDDEF_CMD_MAX
}auddef_qconcert_plus_reverb_config_cmd_type;

/*input to be sent by UI specifying the type and value of the param to be set*/
typedef struct {
  auddef_qconcert_plus_reverb_config_cmd_type cmd_id;
  uint16 data; 
}auddef_qconcert_plus_reverb_config_cmd_params_type;

/*===========================================================================

                     EXPOSED API FUNCTION PROTYPES

===========================================================================*/

/*===========================================================================

FUNCTION snd_set_qconcert_plus_reverb_params

DESCRIPTION
  Configures the mode for the Qconcert 3D audio expander.

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
);

#endif /* AUDQCONCERT_H */

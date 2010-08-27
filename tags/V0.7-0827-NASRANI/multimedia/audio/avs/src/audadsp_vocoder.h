#ifndef AUDADSPVOCSTATE_H
#define AUDADSPVOCSTATE_H
/*===========================================================================

           Q D S P 4   M I X E R    V O C O D E R    D R I V E R
           S T A T E   C O N T R O L L E R   H E A D E R   F I L E

DESCRIPTION
  This header file contains definitions for functions and data used by the
  QDSP2 Vocoder Driver State Machine.

REFERENCES
  None.

Copyright (c) 1999 - 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
   
$Header: //source/qcom/qct/multimedia/audio/6k/ver1/avs/main/latest/src/audadsp_vocoder.h#1 $ $DateTime: 2008/04/24 18:15:13 $ $Author: yuanz $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/07/08    az     Included the in call midi and the new in call architecture.
09/01/06    hz     Added FEATURE_AVS_INCALL_MIXER_ADEC_PB protection.
08/03/06    hz     Initial Version.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"           /* Customer feature specific              */
#include "target.h"             /* Target specific definitions            */
#include "comdef.h"             /* Definitions for byte, word, etc.       */
#include "rex.h"                /* definition of REX data types           */
#include "voc.h"                /* Vocoder interface definition           */
#include "voci.h"               /* Internal Vocoder Task Definitions      */
#include "qdsp.h"               /* QDSP Services                          */

#if defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB) || \
    defined(FEATURE_AVS_INCALL_MIXER_CMX)
/* <EJECT> */
/*===========================================================================

                    D A T A   D E F I N I T I O N S
               
===========================================================================*/

/* ENUMs describe the vocoder state transition.
 */
typedef enum {
  AUDADSP_VOCODER_STATE_INIT_ENTRY,        
                                   
  AUDADSP_VOCODER_STATE_INIT,        
  AUDADSP_VOCODER_STATE_IDLE_ENTRY,  
  AUDADSP_VOCODER_STATE_IDLE,        
  AUDADSP_VOCODER_STATE_ACTIVE_ENTRY,
  AUDADSP_VOCODER_STATE_ACTIVE,      
  AUDADSP_VOCODER_STATE_MAX          

} audadsp_vocoder_state_type;

/* Internal vocoder state control block.
 */
typedef struct {
  uint16                   retry_counter;
  audadsp_vocoder_state_type state;
} audadsp_vocoder_state_ctl_type; 

/* <EJECT> */
/*===========================================================================

                   F U N C T I O N   D E F I N I T I O N S                
  
===========================================================================*/
/*===========================================================================

FUNCTION audadsp_voc_acquire

DESCRIPTION
  This function acquires or disacquire in-call voice module. 

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void audadsp_voc_acquire
(
  boolean acq_flag
);
#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB */
#endif /* AUDADSPVOCSTATE_H */

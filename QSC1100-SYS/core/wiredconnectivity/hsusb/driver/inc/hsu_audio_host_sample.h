#ifndef _HSU_AUDIO_HOST_SAMPLE_H_
#define _HSU_AUDIO_HOST_SAMPLE_H_

/*==============================================================================
  GENERAL DESCRIPTION
   
  EXTERNALIZED FUNCTIONS
  hsu_audio_host_sample()

  INITALIZATION AND SEQUENCING REQUIREMENTS
    
  Copyright (c) 2008, 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: 
  $DateTime: 2008/06/02 12:35:57 $ $Author: merez $
    
  when     who  what, where, why
  -------- ---  ---------------------------------------------------------
  05/29/08  me  Initial version
    
==============================================================================*/
#include "jtypes.h"
/*==============================================================================
  Forward Declarations
==============================================================================*/

/*==============================================================================
  Constants
==============================================================================*/

/*==============================================================================
  Macros
==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/

/*==============================================================================
  PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/

/*===========================================================================
FUNCTION hsu_audio_host_sample_init

DESCRIPTION
This function initializes the audio host sample application 

DEPENDENCIES
This function should be called after hsu_conf_sel_stack_utils_setup_stack 
has been called.

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
jresult_t hsu_audio_host_sample_init(void);


/*===========================================================================
FUNCTION hsu_audio_host_sample_uninit

DESCRIPTION
This function un-initializes the audio host sample application 

DEPENDENCIES

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void hsu_audio_host_sample_uninit(void);

#endif /* _HSU_AUDIO_HOST_SAMPLE_H_ */

#ifndef HSU_CD_SAMPLE_LOOPBACK_H
#define HSU_CD_SAMPLE_LOOPBACK_H

/*==============================================================================

                      High Speed USB Sample Loopback Class Driver Header File
  
GENERAL DESCRIPTION
Header file of loopback class Driver.

EXTERNALIZED FUNCTIONS
hsu_cd_sample_loopback_init

Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

  EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
$Header: 
$DateTime: 


when      who     what, where, why
--------  ---     ----------------------------------------------------------
06/01/08  tk      created file

==============================================================================*/

#include <jusb_common.h>

/*==============================================================================

FUNCTION      hsu_cd_sample_loopback_init

DESCRIPTION   

  The function is defined in hsu_cd_sample_loopback.c by using macro, DRIVER_USB_DECLARE(hsu_cd_sample_loopback).
  It registers the loopback class driver to USB core stack. This function should be called after 
  hsu_conf_sel_stack_utils_setup_stack has been called. Recommended location where it is called is at the end of 
  hsu_config_selector_init in hsu_config_selector.c

DEPENDENCIES  
  This function should be called after hsu_conf_sel_stack_utils_setup_stack has been called.

RETURN VALUE  
  HSU_JSUCCESS (0) for success, other values for failure.

SIDE EFFECTS  
  None.

==============================================================================*/
jresult_t hsu_cd_sample_loopback_init(void);

#endif /* !HSU_CD_SAMPLE_LOOPBACK_H */
